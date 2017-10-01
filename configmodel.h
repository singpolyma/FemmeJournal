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

	//Q_PROPERTY(bool menstruationStarted READ menstruationStarted WRITE setMenstruationStarted NOTIFY menstruationStartedChanged FINAL)

public:
	explicit ConfigModel(QFile *symptomsFile, QObject *parent = nullptr);
	~ConfigModel();

	const QStringList& allSymptoms() const;
	int addSymptom(QString symptom, bool save = true);

public slots:
	void saveSymptoms();

protected:
	QStringList _allSymptoms;
	QFile *_symptomsFile;
	QTextStream _symptomsStream;

	Q_DISABLE_COPY(ConfigModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(ConfigModel)

#endif // CONFIGMODEL_H
