#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "calendarmodel.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	CalendarModel calendarModel(engine.rootContext());
	engine.rootContext()->setContextProperty("calendarModel", &calendarModel);
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
