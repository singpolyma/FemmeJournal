// This file includes code Copyright (C) 2017 The Qt Company Ltd. dual-licensed
// under the LGPLv3 and GPLv2-or-later from the Qt Labs Calendar module
// Modifications and new code for this project are licensed under the AGPLv3

#include <QDebug>

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

CalendarModel::CalendarModel(QObject *parent) : QAbstractListModel(parent), _dates(daysOnACalendarMonth) {
	QDate _today = QDate::currentDate();
	_selectedDate = _today;
	_month = _today.month();
	_year = _today.year();
	populate(_month, _year, _locale, true);
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
		case PeriodRole:
			return date.day() > 5 && date.day() < 10;
		case CycleDayRole:
			return date.day();
		default:
			break;
		}
	}
	return QVariant();
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
	roles[PeriodRole] = QByteArrayLiteral("period");
	roles[CycleDayRole] = QByteArrayLiteral("cycleDay");
	return roles;
}

QT_END_NAMESPACE
