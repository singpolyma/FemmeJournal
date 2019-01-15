#include <math.h>
#include <QDebug>

#include "statsmodel.h"

StatsModel::StatsModel(QMap<QDate, JournalEntry*> *journalDates, QObject *parent) : QAbstractListModel(parent), _journalDates(journalDates) {
	refresh();
}

int StatsModel::meanCycleLength() const {
	return _meanCycleLength;
}

int StatsModel::meanMenstruationLength() const {
	return _meanMenstruationLength;
}

int StatsModel::meanOvulationDaysFromEnd() const {
	return _meanOvulationDaysFromEnd;
}

QVariant StatsModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || index.row() >= rowCount()) return QVariant();

	switch(role) {
	case CycleRole:
		return QVariant::fromValue(_recentCycles[index.row()]);
	}

	return QVariant();
}

int StatsModel::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) return 0;
	return _recentCycles.length();
}

QHash<int, QByteArray> StatsModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[CycleRole] = QByteArrayLiteral("cycle");
	return roles;
}

static QPair<double, double> meanAndStddev(QVector<int> v, double defaultValue, int preferredCount) {
	double meanNumerator = 0;
	double stddevNumerator = 0;
	int count = 0;

	for(QVector<int>::const_iterator i = v.cbegin(); i != v.cend(); i++) {
		count++;
		meanNumerator += *i;
		stddevNumerator += pow(*i, 2);
	}

	for(; count < preferredCount; count++) {
		meanNumerator += defaultValue;
		stddevNumerator += pow(defaultValue, 2);
	}

	Q_ASSERT(count);

	double mean = meanNumerator / count;

	return qMakePair(mean, sqrt((stddevNumerator / count) - pow(mean, 2)));
}

static double meanExcludingOutliers(QVector<int> v, double defaultValue, int preferredCount) {
	QPair<double, double> truth = meanAndStddev(v, defaultValue, preferredCount);
	double lowerBound = truth.first - truth.second;
	double upperBound = truth.first + truth.second;

	double numerator = 0;
	int count = 0;

	for(QVector<int>::const_iterator i = v.cbegin(); i != v.cend(); i++) {
		if(*i < lowerBound || *i > upperBound) continue;
		numerator += *i;
		count++;
	}

	for(; count < preferredCount; count++) {
		numerator += defaultValue;
	}

	Q_ASSERT(count);

	return numerator / count;
}

void StatsModel::refresh() {
	qDeleteAll(_recentCycles);
	_recentCycles.clear();

	QVector<int> cycles;
	QVector<int> menstruations;
	QVector<int> ovulations;

	const QDate *lastBegan = NULL;
	const QDate *lastEnded = NULL;
	const QDate *lastOPKnegative = NULL;
	const QDate *lastOPKpositive = NULL;

	for(
		QMap<QDate,JournalEntry*>::const_iterator i = (_journalDates->end() - 1);
		i != (_journalDates->begin() - 1) && _recentCycles.length() < 12;
		i--
	) {
		if(i.value()->menstruationStarted()) {
			if(lastBegan) {
				QVariant menstrualLength;
				QVariant ovulationDaysFromEnd;
				bool opkPositive = false;

				if(lastEnded) {
					menstrualLength = i.key().daysTo(*lastEnded) + 1;
					menstruations.append(menstrualLength.toInt());
				}

				if(lastOPKpositive) {
					opkPositive = true;
					ovulationDaysFromEnd = i.key().daysTo(*lastBegan);
					ovulations.append(ovulationDaysFromEnd.toInt());
				} else if(lastOPKnegative && lastOPKnegative->daysTo(*lastBegan) < 14) {
					// No positive OPK, but a late negative, so ovulation
					// likely took place later than this
					ovulationDaysFromEnd = lastOPKnegative->daysTo(*lastBegan) - 1;;
					ovulations.append(ovulationDaysFromEnd.toInt());
				}

				cycles.append(i.key().daysTo(*lastBegan));
				_recentCycles.append(new Cycle(i.key(), i.key().daysTo(*lastBegan), menstrualLength, ovulationDaysFromEnd, opkPositive));
			}

			lastOPKpositive = NULL;
			lastOPKnegative = NULL;
			lastEnded = NULL;
			lastBegan = &i.key();
		}

		if(i.value()->menstruationStopped()) {
			lastEnded = &i.key();
		}

		if(lastBegan && i.value()->property("opk") == JournalEntry::OPKPositive) {
			lastOPKpositive = &i.key();
		}

		if(lastBegan && !lastOPKnegative && i.value()->property("opk") == JournalEntry::OPKNegative) {
			lastOPKnegative = &i.key();
		}
	}

	_meanCycleLength = round(meanExcludingOutliers(cycles, 28, 12));
	_meanMenstruationLength = round(meanExcludingOutliers(menstruations, 4, 12));
	_meanOvulationDaysFromEnd = round(meanExcludingOutliers(ovulations, 14, 12));
	if(!_meanOvulationDaysFromEnd || _meanOvulationDaysFromEnd == 14) _meanOvulationDaysFromEnd = _meanCycleLength - 14;

	emit refreshed();
	beginRemoveRows(QModelIndex(), 0, qMax(rowCount() - 1, 0));
	endRemoveRows();
	beginInsertRows(QModelIndex(), 0, qMax(rowCount() - 1, 0));
	endInsertRows();
}
