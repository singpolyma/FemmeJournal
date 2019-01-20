// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#ifndef QCALPARSER_H
#define QCALPARSER_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QTimer>
#include <QDate>

#include "configmodel.h"
#include "journalentry.h"

class QCalParser : public QObject {
	Q_OBJECT
public:
	QCalParser(QFile *file, ConfigModel *config, QObject *parent = 0);
	~QCalParser();

public slots:
	void parse();
	void addJournalEntry(QDate date, JournalEntry *entry);
	void changeDataFilePath(QString newPath);
	void changeWeightUnit(QString oldUnit, QString newUnit);
	void changeTemperatureUnit(QString oldUnit, QString newUnit);
	void delaySave();
	void save();

signals:
	void doneParse();
	void newJournalEntry(QDate date, JournalEntry *entry);

protected:
	void parseBlock(QString weightUnit, QString temperatureUnit);
	QStringList saveEntry(QString weightUnit, QString temperatureUnit, JournalEntry* entry);

	QList<QVariant> m_eventList;
	QTextStream m_dataStream;
	QFile *_file;
	QTimer _timer;
	ConfigModel *_config;
	QString _weightUnit;
	QString _temperatureUnit;
};

#endif // QCALPARSER_H
