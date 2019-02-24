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
	Date firstDateToDisplay = firstDayOfMonthDate.addDays(-difference);

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
	_statsModel = new StatsModel(config, &_journalDates, this);
}

void CalendarModel::ready() {
	_ready = true;
	refreshMenstrualData();
}

void CalendarModel::setSelectedDate(Date date) {
	_selectedDate = date;
	emit selectedDateChanged();
	emit selectedJournalChanged();
}

JournalEntry *CalendarModel::selectedJournal() {
	return entryOf(_selectedDate);
}

JournalEntry *CalendarModel::entryOf(const Date &date) {
	Q_ASSERT(date.isValid());
	JournalEntry *entry = _journalDates.value(date, NULL);
	if(!entry) {
		entry = new JournalEntry(_config, this);
		_journalDates.insert(date, entry);
		addJournalEntry(date, entry, true);
		emit newJournalEntry(date, entry);
	}

	return entry;
}

JournalEntry *CalendarModel::journalForToday() {
	return entryOf(QDate::currentDate());
}

void CalendarModel::addJournalEntry(Date date, JournalEntry *entry, bool empty) {
	Q_ASSERT(entry);

	entry->setParent(this);
	_journalDates.insert(date, entry);

	// Use a timer to work-around false binding loop detection
	if(date == _selectedDate) QTimer::singleShot(0, this, SIGNAL(selectedJournalChanged()));
	if(date == QDate::currentDate()) QTimer::singleShot(0, this, SIGNAL(journalForTodayChanged()));

	connect(entry, SIGNAL(menstruationStartedChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(menstruationStoppedChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(opkChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, SIGNAL(temperatureChanged()), this, SLOT(refreshMenstrualData()));
	connect(entry, &JournalEntry::changed, this, [=]() { emit journalChanged(date, entry->icsLines()); });

	if(!empty) refreshMenstrualData();
	refreshJournalData();
}

Date CalendarModel::nextCycle() {
	// Predict future cycles based on mean length alone for now

	QDate today = QDate::currentDate();
	int cycleLeft = _statsModel->meanCycleLength() - cycleDay(today, false);
	return today.addDays(cycleLeft + 1);
}

bool CalendarModel::menstruatingToday() {
	QDate today = QDate::currentDate();
	return menstruating(today, false);
}

void CalendarModel::refreshMenstrualData() {
	if(!_ready) return;

	for(
		QMap<Date,JournalEntry*>::const_iterator i = (_journalDates.end() - 1);
		i != (_journalDates.begin() - 1);
		i--
	) {
		if(i.value()->menstruationStarted()) {
			_lastRecordedMenstruation = i.key();
			break;
		}
	}

	_statsModel->refresh();

	emit nextCycleChanged();
	emit menstruatingTodayChanged();

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

Date CalendarModel::dateAt(int index) const {
	return _dates.value(index);
}

int CalendarModel::indexOfToday() const {
	QDate date = QDate::currentDate();
	if (date < _dates.first() || date > _dates.last())
		return -1;
	return qMax(qint64(0), _dates.first().daysTo(date));
}

QVariant CalendarModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() && index.row() < daysOnACalendarMonth) {
		Date date = _dates.at(index.row());
		switch (role) {
		case DateRole:
			return QVariant::fromValue(date);
		case DayRole:
			return date.day();
		case WeekNumberRole:
			return date.weekNumber();
		case MonthRole:
			return date.month() - 1;
		case YearRole:
			return date.year();
		case MenstruatingRole:
			return menstruating(date);
		case CycleDayRole:
			return cycleDay(date);
		case FertilityRole: {
			QContiguousCache<QPair<Date,double>> temperatureWindow(7);
			int cday = cycleDay(date);
			Date nextCycle = date.addDays(_statsModel->meanCycleLength() - cday + 1);
			Date ovulation = nextCycle.addDays(_statsModel->meanOvulationDaysFromEnd() * -1);
			Date temperaturePrediction;

			for(
				QMap<Date,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date.addDays(-cday + 2));
				i != _journalDates.end();
				i++
			) {
				temperatureWindow.append(qMakePair(i.key(), i.value()->property("temperature").toDouble()));
				if(i.value()->property("opk") == JournalEntry::OPKPositive) {
					temperaturePrediction = Date();
					ovulation = i.key();
					break;
				}
				if(!temperaturePrediction.isValid()) {
					temperaturePrediction = StatsModel::predictOvulationFromTemperature(_config->property("temperatureUnit").toString(), temperatureWindow);
				}
				if(i.value()->menstruationStarted()) {
					ovulation = i.key().addDays(_statsModel->meanOvulationDaysFromEnd() * -1);
					break;
				}
			}

			if(temperaturePrediction.isValid()) ovulation = temperaturePrediction;

			QMap<Date,JournalEntry*>::const_iterator i = _journalDates.lowerBound(ovulation);
			if(i.key() == ovulation) {
				// If there is data for the predicted day, advance past any negative OPK values
				for(
					;
					i != _journalDates.end() && i.value()->property("opk") == JournalEntry::OPKNegative;
					i++
				) {
					ovulation = ovulation.addDays(1);
				}
			}

			int daysTo = ovulation.daysTo(date);
			if(daysTo == 0) return "ovulated";
			if(daysTo > 0 && daysTo < 2) return "fertile";
			if(daysTo < 0 && daysTo >= -5) return "fertile";
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

int CalendarModel::cycleDay(Date date, bool rollover) const {
	for(
		QMap<Date,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
		i != (_journalDates.begin() - 1);
		i--
	) {
		if(i == _journalDates.end() || i.key() > date) continue;
		if(i.value()->menstruationStarted()) {
			if(_lastRecordedMenstruation < date && rollover) {
				// Predict future cycles based on mean length alone for now
				return (i.key().daysTo(date) % _statsModel->meanCycleLength()) + 1;
			} else {
				return i.key().daysTo(date) + 1;
			}
		}
	}

	return 0;
}

bool CalendarModel::menstruating(Date date, bool rollover) const {
	int cday = cycleDay(date, rollover);
	if(!cday) return false;

	for(
		QMap<Date,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
		i != _journalDates.end() && !i.value()->menstruationStarted();
		i++
	) {
		if(i.value()->menstruationStopped()) return true;
	}

	for(
		QMap<Date,JournalEntry*>::const_iterator i = _journalDates.lowerBound(date);
		i != (_journalDates.begin() - 1);
		i--
	) {
		if(i == _journalDates.end()) continue;
		if(i.value()->menstruationStarted() || i.key().daysTo(date) >= cday) break;
		if(i.value()->menstruationStopped()) return false;
	}

	return cday <= _statsModel->meanMenstruationLength();
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
