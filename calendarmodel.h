// This file includes code Copyright (C) 2017 The Qt Company Ltd. dual-licensed
// under the LGPLv3 and GPLv2-or-later from the Qt Labs Calendar module
// Modifications and new code for this project are licensed under the AGPLv3

#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qlocale.h>
#include <QtQml/qqml.h>

#include "configmodel.h"
#include "journalentry.h"

QT_BEGIN_NAMESPACE

class CalendarModel : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QDate selectedDate MEMBER _selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged FINAL)
	Q_PROPERTY(JournalEntry *selectedJournal READ selectedJournal NOTIFY selectedJournalChanged FINAL)
	Q_PROPERTY(QDate nextCycle READ nextCycle NOTIFY nextCycleChanged FINAL)
	Q_PROPERTY(bool menstruatingToday READ menstruatingToday NOTIFY menstruatingTodayChanged FINAL)

	Q_PROPERTY(int month READ month WRITE setMonth NOTIFY monthChanged FINAL)
	Q_PROPERTY(int year READ year WRITE setYear NOTIFY yearChanged FINAL)
	Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged FINAL)
	Q_PROPERTY(QString title MEMBER _title NOTIFY titleChanged FINAL)
	Q_PROPERTY(int count READ rowCount CONSTANT FINAL)

public:
	explicit CalendarModel(ConfigModel *config = nullptr, QObject *parent = nullptr);

	void setSelectedDate(QDate date);
	JournalEntry *selectedJournal();

	QDate nextCycle();
	bool menstruatingToday();

	int month() const;
	void setMonth(int month);

	int year() const;
	void setYear(int year);

	QLocale locale() const;
	void setLocale(const QLocale &locale);

	Q_INVOKABLE JournalEntry *entryOf(const QDate &date);
	Q_INVOKABLE QDate dateAt(int index) const;
	Q_INVOKABLE int indexOf(const QDate &date) const;

	enum {
		DateRole = Qt::UserRole + 1,
		DayRole,
		WeekNumberRole,
		MonthRole,
		YearRole,
		MenstruatingRole,
		CycleDayRole,
		FertilityRole,
		JournalEntryRole
	};

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
	void selectedDateChanged();
	void selectedJournalChanged();
	void nextCycleChanged();
	void menstruatingTodayChanged();
	void monthChanged();
	void yearChanged();
	void localeChanged();
	void titleChanged();
	void newJournalEntry(QDate, JournalEntry*);
	void journalChanged();

public slots:
	void refreshMenstrualData();
	void refreshJournalData();
	void addJournalEntry(QDate date, JournalEntry *entry);
	void ready();

protected:
	bool populate(int month, int year, const QLocale &locale, bool force = false);
	void populateMeanCycleTimes();
	int cycleDay(QDate date, bool rollover = true) const;

	int _month;
	int _year;
	QString _title;
	QLocale _locale;
	QVector<QDate> _dates;
	QMap<QDate, JournalEntry*> _journalDates;
	QDate _selectedDate;
	QDate _lastRecordedMenstruation;
	int _meanCycleLength;
	int _meanMenstruationLength;
	int _meanOvulationDaysFromEnd;
	bool _ready;
	ConfigModel *_config;

	Q_DISABLE_COPY(CalendarModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(CalendarModel)

#endif // CALENDARMODEL_H
