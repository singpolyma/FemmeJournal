// This file includes code Copyright (C) 2017 The Qt Company Ltd. dual-licensed
// under the LGPLv3 and GPLv2-or-later from the Qt Labs Calendar module
// Modifications and new code for this project are licensed under the AGPLv3

#include <QTimer>
#include <QDebug>
#include <math.h>

#include "calendarmodel.h"

namespace {
	static const int daysInAWeek = 7;
	static const int weeksOnACalendarMonth = 6;
	static const int daysOnACalendarMonth = daysInAWeek * weeksOnACalendarMonth;
}

QT_BEGIN_NAMESPACE

bool CalendarModel::populate(int m, int y, const QLocale &l, bool force) {
	if (!force && m == _month && y == _year && l.firstDayOfWeek() == _locale.firstDayOfWeek())
		return false;

	// The actual first (1st) day of the month.
	QDate firstDayOfMonthDate(y, m, 1);
	int difference = ((firstDayOfMonthDate.dayOfWeek() - l.firstDayOfWeek()) + 7) % 7;
	// The first day to display should never be the 1st of the month, as we want some days from
	// the previous month to be visible.
	if (difference == 0)
		difference += 7;
	QDate firstDateToDisplay = firstDayOfMonthDate.addDays(-difference);

	for (int i = 0; i < daysOnACalendarMonth; ++i)
		_dates[i] = firstDateToDisplay.addDays(i);

	setProperty("title", l.standaloneMonthName(m) + QStringLiteral(" ") + QString::number(y));

	return true;
}

CalendarModel::CalendarModel(ConfigModel *config, QObject *parent) : QAbstractListModel(parent), _dates(daysOnACalendarMonth), _journalDates(), _ready(false), _config(config) {
	Q_ASSERT(config);
	QDate _today = QDate::currentDate();
	_selectedDate = _today;
	_month = _today.month();
	_year = _today.year();
	populate(_month, _year, _locale, true);
	populateMeanCycleTimes();
}

void CalendarModel::ready() {
	_ready = true;
	refreshMenstrualData();
}

void CalendarModel::setSelectedDate(QDate date) {
	_selectedDate = date;
	emit selectedDateChanged();
	emit selectedJournalChanged();
}

JournalEntry *CalendarModel::selectedJournal() {
	return entryOf(_selectedDate);
}

JournalEntry *CalendarModel::entryOf(const QDate &date) {
	JournalEntry *entry = _journalDates.value(date, NULL);
	if(!entry) {
		entry = new JournalEntry(_config, this);
		_journalDates.insert(date, entry);
		addJournalEntry(date, entry);
		emit newJournalEntry(date, entry);
	}

	return entry;
}

void CalendarModel::addJournalEntry(QDate date, JournalEntry *entry) {
	Q_ASSERT(entry);

	entry->setParent(this);
	_journalDates.insert(date, entry);

	// Use a timer to work-around false binding loop detection
	if(date == _selectedDate) QTimer::singleShot(0, this, SIGNAL(selectedJournalChanged()));

	connect(entry, SIGNAL(menstruationStartedChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(menstruationStoppedChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(opkChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(changed()), this, SIGNAL(journalChanged()));

	refreshMenstrualData();
	refreshJournalData();
}

QDate CalendarModel::nextCycle() {
	// Predict future cycles based on mean length alone for now

	QDate today = QDate::currentDate();
	int cycleLeft = _meanCycleLength - cycleDay(today, false);
	return today.addDays(cycleLeft + 1);
}

void CalendarModel::refreshMenstrualData() {
	if(!_ready) return;

	for(
		QMap<QDate,JournalEntry*>::const_iterator i = (_journalDates.end() - 1);
		i != (_journalDates.begin() - 1);
		i--
	) {
		if(i.value()->menstruationStarted()) {
			_lastRecordedMenstruation = i.key();
			break;
		}
	}

	populateMeanCycleTimes();

	emit nextCycleChanged();

	QVector<int> roles({MenstruatingRole, CycleDayRole, FertilityRole});
	emit dataChanged(index(0, 0), index(daysOnACalendarMonth - 1, 0), roles);
}

void CalendarModel::refreshJournalData() {
	QVector<int> roles({JournalEntryRole});
	emit dataChanged(index(0, 0), index(daysOnACalendarMonth - 1, 0), roles);
}

int CalendarModel::month() const {
	return _month;
}

void CalendarModel::setMonth(int month) {
	if (_month != month) {
		if (populate(month, _year, _locale))
			emit dataChanged(index(0, 0), index(daysOnACalendarMonth - 1, 0));
		_month = month;
		emit monthChanged();
	}
}

int CalendarModel::year() const {
	return _year;
}

void CalendarModel::setYear(int year) {
	if (_year != year) {
		if (this->populate(_month, year, _locale))
			emit dataChanged(index(0, 0), index(daysOnACalendarMonth - 1, 0));
		_year = year;
		emit yearChanged();
	}
}

QLocale CalendarModel::locale() const {
	return _locale;
}

void CalendarModel::setLocale(const QLocale &locale) {
	if (_locale != locale) {
		if (populate(_month, _year, locale))
			emit dataChanged(index(0, 0), index(daysOnACalendarMonth - 1, 0));
		_locale = locale;
		emit localeChanged();
	}
}

QDate CalendarModel::dateAt(int index) const {
	return _dates.value(index);
}

int CalendarModel::indexOf(const QDate &date) const {
	if (date < _dates.first() || date > _dates.last())
		return -1;
	return qMax(qint64(0), _dates.first().daysTo(date));
}

QVariant CalendarModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() && index.row() < daysOnACalendarMonth) {
		const QDate date = _dates.at(index.row());
		switch (role) {
		case DateRole:
			return date;
		case DayRole:
			return date.day();
		case WeekNumberRole:
			return date.weekNumber();
		case MonthRole:
			return date.month() - 1;
		case YearRole:
			return date.year();
		case MenstruatingRole: {
			int cday = cycleDay(date);
			if(!cday) return false;

			for(
				QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
				i != _journalDates.end() && !i.value()->menstruationStarted();
				i++
			) {
				if(i.value()->menstruationStopped()) return true;
			}

			for(
				QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
				i != (_journalDates.begin() - 1);
				i--
			) {
				if(i == _journalDates.end()) continue;
				if(i.value()->menstruationStarted() || i.key().daysTo(date) >= cday) break;
				if(i.value()->menstruationStopped()) return false;
			}

			return cday <= _meanMenstruationLength;
		}
		case CycleDayRole:
			return cycleDay(date);
		case FertilityRole: {
			int cday = cycleDay(date);
			QDate nextCycle = date.addDays(_meanCycleLength - cday + 1);

			for(
				QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
				i != _journalDates.end();
				i++
			) {
				if(i.value()->menstruationStarted()) {
					nextCycle = i.key();
					break;
				}
				if(i.value()->property("opk") == JournalEntry::OPKPositive) {
					if(i.key() == date) return "ovulated";
					if(date.daysTo(i.key()) < 5) return "fertile";
					return false;
				}
			}

			for(
				QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
				i != (_journalDates.begin() - 1);
				i--
			) {
				if(i == _journalDates.end()) continue;
				if(i.key().daysTo(date) >= cday) break;
				if(i.value()->property("opk") == JournalEntry::OPKPositive) {
					if(i.key() == date) return "ovulated";
					if(i.key().daysTo(date) < 2) return "fertile";
					return false;
				}
			}

			QDate predictedOvulation = nextCycle.addDays(_meanOvulationDaysFromEnd * -1);

			QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(predictedOvulation);
			if(i.key() == predictedOvulation) {
				// If there is data for the predicted day, advance past any negative OPK values
				for(
					;
					i != _journalDates.end() && i.value()->property("opk") == JournalEntry::OPKNegative;
					i++
				) {
					predictedOvulation = predictedOvulation.addDays(1);
				}
			}

			int daysTo = predictedOvulation.daysTo(date);
			if(daysTo == 0) return "ovulated";
			if(daysTo > 0 && daysTo < 2) return "fertile";
			if(daysTo < 0 && daysTo > -5) return "fertile";
			return false;
		}
		case JournalEntryRole:
			return QVariant::fromValue(_journalDates.value(date));
		default:
			break;
		}
	}
	return QVariant();
}

void CalendarModel::populateMeanCycleTimes() {
	int numerator = 0;
	int count = 0;
	int mNumerator = 0;
	int mCount = 0;
	int ovulationNumerator = 0;
	int ovulationCount = 0;
	const QDate *lastBegan = NULL;
	const QDate *lastEnded = NULL;
	const QDate *lastOPKnegative = NULL;
	const QDate *lastOPKpositive = NULL;

	for(
		QMap<QDate,JournalEntry*>::const_iterator i = (_journalDates.end() - 1);
		i != (_journalDates.begin() - 1) && count < 6;
		i--
	) {
		if(i.value()->menstruationStarted()) {
			if(lastBegan) {
				numerator += i.key().daysTo(*lastBegan);
				count++;
			}

			if(lastEnded) {
				mNumerator += i.key().daysTo(*lastEnded) + 1;
				mCount++;
				lastEnded = NULL;
			}

			if(lastBegan && lastOPKnegative) {
				if(!lastOPKpositive && lastOPKnegative->daysTo(*lastBegan) < 14) {
					// No positive OPK, but a late negative, so ovulation
					// likely took place later than this
					ovulationNumerator += lastOPKnegative->daysTo(*lastBegan) - 1;
					ovulationCount++;
				}
				lastOPKnegative = NULL;
			}

			lastOPKpositive = NULL;
			lastBegan = &i.key();
		}

		if(i.value()->menstruationStopped()) {
			lastEnded = &i.key();
		}

		if(lastBegan && i.value()->property("opk") == JournalEntry::OPKPositive) {
			lastOPKpositive = &i.key();
			ovulationNumerator += i.key().daysTo(*lastBegan);
			ovulationCount++;
		}

		if(lastBegan && !lastOPKnegative && i.value()->property("opk") == JournalEntry::OPKNegative) {
			lastOPKnegative = &i.key();
		}
	}

	// TODO: discount very long cycles (likely a skipped period)
	_meanCycleLength = count == 0 ? 28 : round((double)numerator / count);
	_meanMenstruationLength = mCount == 0 ? 4 : round((double)mNumerator / mCount);
	_meanOvulationDaysFromEnd = ovulationCount == 0 ? _meanCycleLength - 14 : round((double)ovulationNumerator / ovulationCount);
}

int CalendarModel::cycleDay(QDate date, bool rollover) const {
	for(
		QMap<QDate,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
		i != (_journalDates.begin() - 1);
		i--
	) {
		if(i == _journalDates.end() || i.key() > date) continue;
		if(i.value()->menstruationStarted()) {
			if(_lastRecordedMenstruation < date && rollover) {
				// Predict future cycles based on mean length alone for now
				return (i.key().daysTo(date) % _meanCycleLength) + 1;
			} else {
				return i.key().daysTo(date) + 1;
			}
		}
	}

	return 0;
}

int CalendarModel::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) return 0;
	return daysOnACalendarMonth;
}

QHash<int, QByteArray> CalendarModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[DateRole] = QByteArrayLiteral("date");
	roles[DayRole] = QByteArrayLiteral("day");
	roles[WeekNumberRole] = QByteArrayLiteral("weekNumber");
	roles[MonthRole] = QByteArrayLiteral("month");
	roles[YearRole] = QByteArrayLiteral("year");
	roles[MenstruatingRole] = QByteArrayLiteral("menstruating");
	roles[CycleDayRole] = QByteArrayLiteral("cycleDay");
	roles[FertilityRole] = QByteArrayLiteral("fertility");
	roles[JournalEntryRole] = QByteArrayLiteral("journalEntry");
	return roles;
}

QT_END_NAMESPACE
