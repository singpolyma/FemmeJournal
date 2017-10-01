#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QtCore/QObject>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class ConfigModel : public QObject
{
	Q_OBJECT

	//Q_PROPERTY(bool menstruationStarted READ menstruationStarted WRITE setMenstruationStarted NOTIFY menstruationStartedChanged FINAL)

public:
	explicit ConfigModel(QObject *parent = nullptr);

	const QStringList& allSymptoms() const;
	int addSymptom(QString symptom);

protected:
	QStringList _allSymptoms;

	Q_DISABLE_COPY(ConfigModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(ConfigModel)

#endif // CONFIGMODEL_H
