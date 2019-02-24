#include <math.h>
#include <QDebug>

#include "statsmodel.h"

StatsModel::StatsModel(ConfigModel *config, QMap<Date, JournalEntry*> *journalDates, QObject *parent) : QAbstractListModel(parent), _config(config), _journalDates(journalDates) {
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

static double meanExcludingOutliers(QVector<int> v, double defaultValue, int preferredCount) {
	double lowerBound = defaultValue - (defaultValue * 0.5);
	double upperBound = defaultValue * 1.5;

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

Date StatsModel::predictOvulationFromTemperature(QString temperatureUnit, QContiguousCache<QPair<Date,double>> &temperatureWindow) {
	if(temperatureWindow.size() < 7) return Date();
	temperatureWindow.normalizeIndexes();
	int firstIndex = temperatureWindow.firstIndex();

	QPair<Date,double> last = temperatureWindow.at(firstIndex + 6);
	if(last.second == 0) return Date();

	double sixDayMean = 0;
	int count = 0;
	for(int i = firstIndex + 5; i >= firstIndex; i--) {
		QPair<Date,double> sample = temperatureWindow.at(i);
		if(sample.first.daysTo(last.first) > 6) break;

		if(sample.second != 0) {
			sixDayMean += sample.second;
			count++;
		}
	}

	if(count < 4) return Date();

	sixDayMean /= count;

	double increment = temperatureUnit == "f" ? 0.4 : 0.2;
	if(last.second - sixDayMean > increment) return last.first;

	return Date();
}

void StatsModel::refresh() {
	qDeleteAll(_recentCycles);
	_recentCycles.clear();

	QVector<int> cycles;
	QVector<int> menstruations;
	QVector<int> ovulations;
	QContiguousCache<QPair<Date,double>> temperatureWindow(7);

	const Date *lastBegan = NULL;
	const Date *lastEnded = NULL;
	const Date *lastOPKnegative = NULL;
	const Date *lastOPKpositive = NULL;
	Date lastTemperaturePrediction;

	for(
		QMap<Date,JournalEntry*>::const_iterator i = (_journalDates->end() - 1);
		i != (_journalDates->begin() - 1) && _recentCycles.length() < 12;
		i--
	) {
		Date temperaturePrediction;
		temperatureWindow.prepend(qMakePair(i.key(), i.value()->property("temperature").toDouble()));
		if((temperaturePrediction = predictOvulationFromTemperature(_config->property("temperatureUnit").toString(), temperatureWindow)).isValid()) {
			lastTemperaturePrediction = temperaturePrediction;
		}

		if(i.value()->menstruationStarted()) {
			if(lastBegan) {
				QVariant menstrualLength;
				int ovulationDaysFromEnd;
				bool opkPositive = false;

				if(lastEnded) {
					menstrualLength = i.key().daysTo(*lastEnded) + 1;
					menstruations.append(menstrualLength.toInt());
				}

				if(lastOPKpositive) {
					opkPositive = true;
					ovulationDaysFromEnd = lastOPKpositive->daysTo(*lastBegan);
					ovulations.append(ovulationDaysFromEnd);
				} else if(lastTemperaturePrediction.isValid()) {
					ovulationDaysFromEnd = lastTemperaturePrediction.daysTo(*lastBegan);
					ovulations.append(ovulationDaysFromEnd);
				} else if(lastOPKnegative && lastOPKnegative->daysTo(*lastBegan) < 14) {
					// No positive OPK, but a late negative, so ovulation
					// likely took place later than this
					ovulationDaysFromEnd = lastOPKnegative->daysTo(*lastBegan) - 1;
					ovulations.append(ovulationDaysFromEnd);
				}

				cycles.append(i.key().daysTo(*lastBegan));
				_recentCycles.append(new Cycle(i.key(), i.key().daysTo(*lastBegan), menstrualLength, ovulationDaysFromEnd, opkPositive));
			}

			lastOPKpositive = NULL;
			lastOPKnegative = NULL;
			lastTemperaturePrediction = Date();
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
