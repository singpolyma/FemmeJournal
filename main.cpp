#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>
#include <QThread>

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

	QThread parserThread;
	QCalParser journalParser(&journalFile);
	journalParser.moveToThread(&parserThread);

	CalendarModel calendarModel;
	QObject::connect(&calendarModel, SIGNAL(newJournalEntry(QDate,JournalEntry*)), &journalParser, SLOT(addJournalEntry(QDate,JournalEntry*)));
	QObject::connect(&journalParser, SIGNAL(newJournalEntry(QDate,JournalEntry*)), &calendarModel, SLOT(addJournalEntry(QDate,JournalEntry*)));
	QObject::connect(&calendarModel, SIGNAL(journalChanged()), &journalParser, SLOT(delaySave()));
	QObject::connect(&parserThread, SIGNAL(started()), &journalParser, SLOT(parse()));
	QObject::connect(QApplication::instance(), SIGNAL(aboutToQuit()), &parserThread, SLOT(quit()));
	parserThread.start();

	qmlRegisterType<JournalEntry>("net.singpolyma.thefertilecycle", 1, 0, "JournalEntry");

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("calendarModel", &calendarModel);
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
