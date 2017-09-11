// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#ifndef QCALPARSER_H
#define QCALPARSER_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QDate>

#include "journalentry.h"

class QCalParser : public QObject {
	Q_OBJECT
public:
	QCalParser(QFile *file, QObject *parent = 0);
	~QCalParser();

public slots:
	void parse();
	void addJournalEntry(QDate date, JournalEntry *entry);
	void save();

signals:
	void newJournalEntry(QDate date, JournalEntry *entry);

protected:
	void parseBlock();
	QStringList saveEntry(JournalEntry* entry);

	QList<QVariant> m_eventList;
	QTextStream m_dataStream;
	QFile *_file;
};

#endif // QCALPARSER_H
