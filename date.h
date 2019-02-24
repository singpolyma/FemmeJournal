#ifndef DATE_H
#define DATE_H

#include <QDate>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

// QDate doesn't pass to QML safely, so use this
class Date
{
Q_GADGET

public:
	Date(unsigned year, unsigned month, unsigned day);
	Date(const QDate date);
	Date(const Date &date);
	Date();

	inline unsigned day() const {
		return _qdate.day();
	}

	inline unsigned month() const {
		return _qdate.month();
	}

	inline unsigned year() const {
		return _qdate.year();
	}

	inline unsigned weekNumber() const {
		return _qdate.weekNumber();
	}

	Q_INVOKABLE qint64 daysTo(const QDateTime &datetime) const;
	qint64 daysTo(const Date &date) const {
		return _qdate.daysTo(date._qdate);
	}

	inline Date addDays(int days) const {
		return _qdate.addDays(days);
	}

	inline bool isValid() const {
		return _qdate.isValid();
	}

	Q_INVOKABLE QDateTime asDateTime() const;

	friend inline bool operator<(const Date &lhs, const Date &rhs) {
		return lhs._qdate < rhs._qdate;
	}

	friend inline bool operator>(const Date &lhs, const Date &rhs) {
		return lhs._qdate > rhs._qdate;
	}

	friend inline bool operator==(const Date &lhs, const Date &rhs) {
		return lhs._qdate == rhs._qdate;
	}

protected:
	QDate _qdate;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Date)

#endif // DATE_H
