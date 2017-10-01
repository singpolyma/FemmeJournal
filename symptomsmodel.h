#ifndef SYMPTOMSMODEL_H
#define SYMPTOMSMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtQml/qqml.h>

#include "configmodel.h"

QT_BEGIN_NAMESPACE

class SymptomsModel : public QAbstractListModel
{
	Q_OBJECT

public:
	explicit SymptomsModel(ConfigModel *config = nullptr, QObject *parent = nullptr);

	enum SymptomSeverity {
		Unknown,
		Light,
		Medium,
		Heavy
	};

	Q_ENUM(SymptomSeverity)

	enum Roles {
		SymptomRole = Qt::UserRole + 1,
		SeverityRole
	};

	Q_ENUM(Roles)

	QHash<int, QByteArray> roleNames() const override;
	Q_INVOKABLE bool setData(const int idx, const QVariant &value, int role);
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	Q_INVOKABLE void readSymptoms(void *ret);
	void setSymptomSeverity(QString symptom, enum SymptomSeverity severity);

protected:
	QMap<QString, enum SymptomSeverity> _symptoms;
	ConfigModel *_config;

	Q_DISABLE_COPY(SymptomsModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(SymptomsModel)

#endif // SYMPTOMSMODEL_H
