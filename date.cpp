#include "date.h"

#include <QDebug>

Date::Date(unsigned year, unsigned month, unsigned day) :
	Date(QDate(year, month, day)) { }

Date::Date(const QDate date) : _qdate(date) { }
Date::Date(const Date &date) : Date(date._qdate) { }
Date::Date() : Date(QDate()) { /* invalid */ }

qint64 Date::daysTo(const QDateTime &datetime) const {
	return _qdate.daysTo(datetime.date());
}

QDateTime Date::asDateTime() const {
	return QDateTime(_qdate);
}
