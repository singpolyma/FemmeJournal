// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#include "qcalparser.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>

QCalParser::QCalParser(QFile *file, QObject *parent) : QObject(parent), m_dataStream() {
	_file = file;
	m_dataStream.setDevice(_file);
}

QCalParser::~QCalParser() {
	_file->close();
}

void QCalParser::addJournalEntry(QDate date, JournalEntry *entry) {
	Q_ASSERT(entry);
	entry->setParent(this);
	m_eventList.append(QVariant::fromValue(QPair<QDate,JournalEntry*>(date, entry)));
}

void QCalParser::save() {
	m_dataStream.seek(0);
	_file->seek(0);
	_file->resize(0);

	for(
		QList<QVariant>::iterator i = m_eventList.begin();
		i != m_eventList.end();
		i++
	) {
		if(i->typeName() != QString("QPair<QDate,JournalEntry*>")) {
			QPair<QDate,JournalEntry*> v = i->value<QPair<QDate,JournalEntry*>>();

			m_dataStream << "BEGIN:VJOURNAL" << endl;
			m_dataStream << "DTSTART:" << v.first.toString("yyyyMMdd") << endl;
			saveEntry(v.second);
			m_dataStream << "END:VJOURNAL" << endl;
		} else if(i->typeName() != QString("JournalEntry*")) {
			m_dataStream << "BEGIN:VJOURNAL" << endl;
			saveEntry(i->value<JournalEntry*>());
			m_dataStream << "END:VJOURNAL" << endl;
		} else {
			m_dataStream << i->toString() << endl;
		}
	}

	m_dataStream.flush();
}

void QCalParser::saveEntry(JournalEntry *entry) {
	if(entry->menstruationStarted()) m_dataStream << "X-THEFERTILECYCLE-MENSTRUATION:STARTED" << endl;
	if(entry->menstruationStopped()) m_dataStream << "X-THEFERTILECYCLE-MENSTRUATION:STOPPED" << endl;
	if(entry->property("intimate").toBool()) m_dataStream << "X-THEFERTILECYCLE-INTIMATE:YES" << endl;
	if(entry->property("ovulated").toBool()) m_dataStream << "X-THEFERTILECYCLE-OVULATED:POSITIVE" << endl;

	QString note = entry->property("note").toString();
	if(!note.isEmpty()) {
		note.replace("\\", "\\\\");
		note.replace("\n", "\\n");
		note.replace(",", "\\,");
		note.replace(";", "\\;");
		m_dataStream << "DESCRIPTION:" << note << endl;
	}
}

void QCalParser::parse() {
	m_dataStream.seek(0);
	QString line = m_dataStream.readLine();
	while(!line.isNull()) {
		if(line.contains("BEGIN:VJOURNAL")) {
			parseBlock();
		} else {
			m_eventList.append(QVariant::fromValue(line));
		}

		line = m_dataStream.readLine();
	}
}

void QCalParser::parseBlock() {
	QDate date;
	JournalEntry *entry = new JournalEntry(this);
	QString line;
	while(!(line = m_dataStream.readLine()).contains(QByteArray("END:VJOURNAL"))) {
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
		emit newJournalEntry(date, entry);
	}
}
