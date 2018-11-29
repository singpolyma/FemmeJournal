#ifndef STATSMODEL_H
#define STATSMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qlocale.h>
#include <QtQml/qqml.h>

#include "journalentry.h"
#include "cycle.h"

QT_BEGIN_NAMESPACE

class StatsModel : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int meanCycleLength READ meanCycleLength NOTIFY refreshed STORED false FINAL)
	Q_PROPERTY(int meanMenstruationLength READ meanMenstruationLength NOTIFY refreshed STORED false FINAL)
	Q_PROPERTY(int meanOvulationDaysFromEnd READ meanOvulationDaysFromEnd NOTIFY refreshed STORED false FINAL)

public:
	explicit StatsModel(QMap<QDate, JournalEntry*> *journalDates, QObject *parent = nullptr);

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
	QMap<QDate, JournalEntry*> *_journalDates;
	QVector<Cycle*> _recentCycles;
	int _meanCycleLength;
	int _meanMenstruationLength;
	int _meanOvulationDaysFromEnd;

	Q_DISABLE_COPY(StatsModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(StatsModel)

#endif // STATSMODEL_H
