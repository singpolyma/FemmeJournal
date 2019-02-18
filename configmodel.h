#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QtCore/QObject>
#include <QtQml/qqml.h>
#include <QFile>
#include <QSaveFile>
#include <QFileInfo>
#include <QTextStream>

QT_BEGIN_NAMESPACE

class ConfigModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString weightUnit MEMBER _weightUnit WRITE setWeightUnit NOTIFY weightUnitChanged FINAL)
	Q_PROPERTY(QString temperatureUnit MEMBER _temperatureUnit WRITE setTemperatureUnit NOTIFY temperatureUnitChanged FINAL)
	Q_PROPERTY(QString dataFilePath MEMBER _dataFilePath NOTIFY dataFilePathChanged FINAL)

public:
	explicit ConfigModel(QString configPath, QString symptomsPath, QObject *parent = nullptr);

	const QStringList& allSymptoms() const;
	int addSymptom(QString symptom, bool save = true);

	void setWeightUnit(QString unit);
	void setTemperatureUnit(QString unit);

	QFileInfo dataFileInfo();

signals:
	void weightUnitChanged(QString oldUnit, QString newUnit);
	void temperatureUnitChanged(QString oldUnit, QString newUnit);
	void dataFilePathChanged(QString newPath);

public slots:
	void saveSymptoms();
	void saveConfig();

protected:
	QString _weightUnit;
	QString _temperatureUnit;
	QString _dataFilePath;
	QStringList _allSymptoms;
	QString _symptomsPath;
	QString _configPath;

	Q_DISABLE_COPY(ConfigModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(ConfigModel)

#endif // CONFIGMODEL_H
