#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QtCore/QObject>
#include <QtQml/qqml.h>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE

class ConfigModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString weightUnit MEMBER _weightUnit NOTIFY weightUnitChanged FINAL)

public:
	explicit ConfigModel(QFile *configFile, QFile *symptomsFile, QObject *parent = nullptr);
	~ConfigModel();

	const QStringList& allSymptoms() const;
	int addSymptom(QString symptom, bool save = true);

	Q_INVOKABLE void readProperty(QByteArray name, void *ret);

public slots:
	void saveSymptoms();
	void saveConfig();

signals:
	void weightUnitChanged();

protected:
	QString _weightUnit;
	QStringList _allSymptoms;
	QFile *_symptomsFile;
	QFile *_configFile;
	QTextStream _symptomsStream;

	Q_DISABLE_COPY(ConfigModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(ConfigModel)

#endif // CONFIGMODEL_H
