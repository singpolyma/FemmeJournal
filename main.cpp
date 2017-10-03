#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>
#include <QThread>
#include <QDebug>

#include "configmodel.h"
#include "calendarmodel.h"
#include "qcalparser.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/FemmeJournal");
	if(!configDir.mkpath(".")) {
		qFatal("Could not make config directory: %s", qUtf8Printable(configDir.path()));
	}

	QFile configFile(configDir.absoluteFilePath("config"));
	if(!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(configFile.fileName()));
	}

	QFile symptomsFile(configDir.absoluteFilePath("symptoms"));
	if(!symptomsFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(symptomsFile.fileName()));
	}

	ConfigModel config(&configFile, &symptomsFile);
	QFileInfo dataFileInfo = config.dataFileInfo();

	if(!dataFileInfo.dir().mkpath(".")) {
		qFatal("Could not make data directory: %s", qUtf8Printable(dataFileInfo.dir().path()));
	}

	QFile journalFile(dataFileInfo.filePath());
	if(!journalFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(journalFile.fileName()));
	}

	QThread parserThread;
	QCalParser journalParser(&journalFile, &config);
	journalParser.moveToThread(&parserThread);

	CalendarModel calendarModel(&config);
	QObject::connect(&config, SIGNAL(dataFilePathChanged(QString)), &journalParser, SLOT(changeDataFilePath(QString)));
	QObject::connect(&calendarModel, SIGNAL(newJournalEntry(QDate,JournalEntry*)), &journalParser, SLOT(addJournalEntry(QDate,JournalEntry*)));
	QObject::connect(&journalParser, SIGNAL(newJournalEntry(QDate,JournalEntry*)), &calendarModel, SLOT(addJournalEntry(QDate,JournalEntry*)));
	QObject::connect(&journalParser, SIGNAL(doneParse()), &calendarModel, SLOT(ready()));
	QObject::connect(&calendarModel, SIGNAL(journalChanged()), &journalParser, SLOT(delaySave()));
	QObject::connect(&parserThread, SIGNAL(started()), &journalParser, SLOT(parse()));
	QObject::connect(QApplication::instance(), SIGNAL(aboutToQuit()), &parserThread, SLOT(quit()));
	parserThread.start();

	qmlRegisterType<JournalEntry>("net.singpolyma.thefertilecycle", 1, 0, "JournalEntry");
	qmlRegisterType<SymptomsModel>("net.singpolyma.thefertilecycle", 1, 0, "SymptomsModel");

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("calendarModel", &calendarModel);
	engine.rootContext()->setContextProperty("config", &config);
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
