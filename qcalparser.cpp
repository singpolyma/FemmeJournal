// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#include "qcalparser.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>

QCalParser::QCalParser(QObject *parent) : QObject(parent), m_dataStream(NULL) {
}

QCalParser::~QCalParser() {
	if(m_dataStream) delete m_dataStream;
}

void QCalParser::populateModel(QObject *model, const char *slot) {
	for(
		QList<QVariant>::iterator i = m_eventList.begin();
		i != m_eventList.end();
		i++
	) {
		if(i->typeName() != QString("QPair<QDate,JournalEntry*>")) continue;
		QPair<QDate,JournalEntry*> v = i->value<QPair<QDate,JournalEntry*>>();

		QMetaObject::invokeMethod(
			model,
			slot,
			Qt::AutoConnection,
			Q_ARG(QDate, v.first),
			Q_ARG(JournalEntry*, v.second)
		);
	}
}

bool QCalParser::parse(const QByteArray &data) {
	if(m_dataStream) delete m_dataStream;

	m_dataStream = new QTextStream(data);
	parse();

	return true;
}

bool QCalParser::parse(QFile *file) {
	if (!file->isOpen()) {
		if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
			return false;
	}

	if(m_dataStream) delete m_dataStream;

	m_dataStream = new QTextStream(file);
	parse();

	return true;
}

void QCalParser::parse() {
	QString line = m_dataStream->readLine();
	while(!line.isNull()) {
		if(line.contains("BEGIN:VJOURNAL")) {
			parseBlock();
		} else {
			m_eventList.append(QVariant::fromValue(line));
		}

		line = m_dataStream->readLine();
	}
}

void QCalParser::parseBlock() {
	QDate date;
	JournalEntry *entry = new JournalEntry(this);
	QString line;
	while(!(line = m_dataStream->readLine()).contains(QByteArray("END:VJOURNAL"))) {
		const int deliminatorPosition = line.indexOf(QChar(':'));
		const QString key   = line.mid(0, deliminatorPosition);
		const QString value = line.mid(deliminatorPosition + 1, -1);

		if(key == QString("DTSTART")) {
			date = QDate::fromString(value, "yyyyMMdd");
		} else if(key == QString("DESCRIPTION")) {
			QString desc = value;
			desc.replace("\\N", "\n");
			desc.replace("\\n", "\n");
			desc.replace("\\,", ",");
			desc.replace("\\;", ";");
			desc.replace("\\\\", "\\");
			entry->setProperty("note", desc);
		} else if(key == QString("X-THEFERTILECYCLE-MENSTRUATION") && value.toCaseFolded() == QString("started")) {
			entry->setMenstruationStarted(true);
		} else if(key == QString("X-THEFERTILECYCLE-MENSTRUATION") && value.toCaseFolded() == QString("stopped")) {
			entry->setMenstruationStopped(true);
		} else if(key == QString("X-THEFERTILECYCLE-INTIMATE")) {
			entry->setProperty("intimate", true);
		} else if(key == QString("X-THEFERTILECYCLE-OVULATED")) {
			entry->setProperty("ovulated", true);
		} else {
			entry->addUnknownLine(line);
		}
	}

	if(date.isNull()) {
		m_eventList.append(QVariant::fromValue(entry));
	} else {
		m_eventList.append(QVariant::fromValue(QPair<QDate,JournalEntry*>(date, entry)));
	}
}
