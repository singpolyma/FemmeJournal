#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>

#include "calendarmodel.h"
#include "qcalparser.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
	if(!dataDir.mkpath(".")) {
		qFatal("Could not make data directory: %s", qUtf8Printable(dataDir.path()));
	}

	QFile journalFile(dataDir.absoluteFilePath("FemmeJournal.ics"));
	if(!journalFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(journalFile.fileName()));
	}

	QCalParser journalParser;
	journalParser.parse(&journalFile);

	CalendarModel calendarModel;
	journalParser.populateModel(&calendarModel, "addJournalDate");

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("calendarModel", &calendarModel);
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
