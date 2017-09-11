// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#include "qcalparser.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtWidgets/QApplication>
#include <QThread>

QCalParser::QCalParser(QFile *file, QObject *parent) : QObject(parent), m_dataStream() {
	_file = file;
	m_dataStream.setDevice(_file);
}

QCalParser::~QCalParser() {
	_file->close();
}

void QCalParser::addJournalEntry(QDate date, JournalEntry *entry) {
	Q_ASSERT(entry);
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
		if(i->typeName() == QString("QPair<QDate,JournalEntry*>")) {
			QPair<QDate,JournalEntry*> v = i->value<QPair<QDate,JournalEntry*>>();

			QStringList lines = saveEntry(v.second);
			if(!lines.empty()) {
				m_dataStream << "BEGIN:VJOURNAL" << endl;
				m_dataStream << "DTSTART:" << v.first.toString("yyyyMMdd") << endl;
				for(QStringList::iterator i = lines.begin(); i != lines.end(); i++) {
					m_dataStream << *i << endl;
				}
				m_dataStream << "END:VJOURNAL" << endl;
			}
		} else if(i->typeName() == QString("JournalEntry*")) {
			m_dataStream << "BEGIN:VJOURNAL" << endl;
			QStringList lines = saveEntry(i->value<JournalEntry*>());
			for(QStringList::iterator i = lines.begin(); i != lines.end(); i++) {
				m_dataStream << *i << endl;
			}
			m_dataStream << "END:VJOURNAL" << endl;
		} else {
			m_dataStream << i->toString() << endl;
		}
	}

	m_dataStream.flush();
}

QStringList QCalParser::saveEntry(JournalEntry *entry) {
	QStringList lines;
	QVariant retVal;

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "menstruationStarted"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) lines << "X-THEFERTILECYCLE-MENSTRUATION:STARTED";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "menstruationStopped"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) lines << "X-THEFERTILECYCLE-MENSTRUATION:STOPPED";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "intimate"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) lines << "X-THEFERTILECYCLE-INTIMATE:YES";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "ovulated"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) lines << "X-THEFERTILECYCLE-OVULATED:POSITIVE";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "note"), Q_ARG(void*, &retVal));
	QString note = retVal.toString();
	if(!note.isEmpty()) {
		note.replace("\\", "\\\\");
		note.replace("\n", "\\n");
		note.replace(",", "\\,");
		note.replace(";", "\\;");
		lines << "DESCRIPTION:" + note;
	}

	return lines;
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
	JournalEntry *entry = new JournalEntry();
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
		entry->setParent(this);
	} else {
		m_eventList.append(QVariant::fromValue(QPair<QDate,JournalEntry*>(date, entry)));
		entry->moveToThread(QApplication::instance()->thread());
		emit newJournalEntry(date, entry);
	}
}
