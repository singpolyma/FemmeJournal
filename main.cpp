#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QIcon>
#include <QDir>
#include <QThread>
#include <QDebug>

#include "configmodel.h"
#include "calendarmodel.h"
#include "qcalparser.h"

#ifdef QT_CHARTS_LIB
#include "temperaturechartmodel.h"
#endif

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#ifdef QT_CHARTS_LIB
	QApplication app(argc, argv);
#else
	QGuiApplication app(argc, argv);
#endif
	app.setWindowIcon(QIcon(":/FemmeJournal.svg"));
	app.setApplicationVersion(VERSION);

	QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/FemmeJournal");
	if(!configDir.mkpath(".")) {
		qFatal("Could not make config directory: %s", qUtf8Printable(configDir.path()));
	}

	ConfigModel config(configDir.absoluteFilePath("config"), configDir.absoluteFilePath("symptoms"));
	QFileInfo dataFileInfo = config.dataFileInfo();

	if(!dataFileInfo.dir().mkpath(".")) {
		qFatal("Could not make data directory: %s", qUtf8Printable(dataFileInfo.dir().path()));
	}

	QThread parserThread;
	QCalParser journalParser(dataFileInfo.filePath(), &config);
	journalParser.moveToThread(&parserThread);

	CalendarModel calendarModel(&config);

	QObject::connect(&journalParser, SIGNAL(newJournalEntry(QDate,JournalEntry*)), &calendarModel, SLOT(addJournalEntry(QDate,JournalEntry*)));
	QObject::connect(&journalParser, SIGNAL(doneParse()), &calendarModel, SLOT(ready()));
	QObject::connect(&calendarModel, SIGNAL(journalChanged(QDate, QStringList)), &journalParser, SLOT(updateJournalLines(QDate, QStringList)));
	QObject::connect(&parserThread, SIGNAL(started()), &journalParser, SLOT(parse()));
	QObject::connect(QApplication::instance(), SIGNAL(aboutToQuit()), &parserThread, SLOT(quit()));

	QQmlApplicationEngine engine;
#ifdef QT_CHARTS_LIB
	TemperatureChartModel temperatureChartModel(&calendarModel);
	QObject::connect(&journalParser, SIGNAL(doneParse()), &temperatureChartModel, SLOT(ready()));
	QObject::connect(&journalParser, &QCalParser::newJournalEntry, &temperatureChartModel, &TemperatureChartModel::addJournalEntry);
	QObject::connect(&calendarModel, &CalendarModel::newJournalEntry, &temperatureChartModel, &TemperatureChartModel::addJournalEntry);
	engine.rootContext()->setContextProperty("temperatureChartModel", &temperatureChartModel);
	engine.rootContext()->setContextProperty("QT_CHARTS_LIB", true);
#else
	engine.rootContext()->setContextProperty("QT_CHARTS_LIB", false);
#endif

	parserThread.start();

	qmlRegisterType<JournalEntry>("net.singpolyma.FemmeJournal", 1, 0, "JournalEntry");
	qmlRegisterType<SymptomsModel>("net.singpolyma.FemmeJournal", 1, 0, "SymptomsModel");

	engine.rootContext()->setContextProperty("calendarModel", &calendarModel);
	engine.rootContext()->setContextProperty("config", &config);
	engine.rootContext()->setContextProperty("VERSION", VERSION);
	engine.rootContext()->setContextProperty("COMMIT", COMMIT);
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
