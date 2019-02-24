#ifndef STATSMODEL_H
#define STATSMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qlocale.h>
#include <QtQml/qqml.h>
#include <QContiguousCache>

#include "date.h"
#include "configmodel.h"
#include "cycle.h"
#include "journalentry.h"

QT_BEGIN_NAMESPACE

class StatsModel : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int meanCycleLength READ meanCycleLength NOTIFY refreshed STORED false FINAL)
	Q_PROPERTY(int meanMenstruationLength READ meanMenstruationLength NOTIFY refreshed STORED false FINAL)
	Q_PROPERTY(int meanOvulationDaysFromEnd READ meanOvulationDaysFromEnd NOTIFY refreshed STORED false FINAL)

public:
	explicit StatsModel(ConfigModel *config, QMap<Date, JournalEntry*> *journalDates, QObject *parent = nullptr);
	static Date predictOvulationFromTemperature(QString temperatureUnit, QContiguousCache<QPair<Date,double>> &temperatureWindow);

	int meanCycleLength() const;
	int meanMenstruationLength() const;
	int meanOvulationDaysFromEnd() const;

	enum {
		CycleRole = Qt::UserRole + 1,
	};

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
	void refreshed();

public slots:
	void refresh();

protected:
	ConfigModel *_config;
	QMap<Date, JournalEntry*> *_journalDates;
	QVector<Cycle*> _recentCycles;
	int _meanCycleLength;
	int _meanMenstruationLength;
	int _meanOvulationDaysFromEnd;

	Q_DISABLE_COPY(StatsModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(StatsModel)

#endif // STATSMODEL_H
