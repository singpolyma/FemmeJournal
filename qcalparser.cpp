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
	QSharedPointer<JournalEntry> event = QSharedPointer<JournalEntry>(new JournalEntry);
	QString line;
	while(!(line = m_dataStream->readLine()).contains(QByteArray("END:VJOURNAL"))) {
		const int deliminatorPosition = line.indexOf(QLatin1Char(':'));
		const QString key   = line.mid(0, deliminatorPosition);
		const QString value = line.mid(deliminatorPosition + 1, -1);

		/*if(key == QLatin1String("DTSTART") || key == QLatin1String("DTEND")) {
			QDateTime utcTime = QDateTime::fromString(value, "yyyyMMdd'T'hhmmss'Z'");
			utcTime.setTimeSpec(Qt::UTC);
			event->setProperty(key, utcTime.toLocalTime());
			continue;
		} else if (key == QLatin1String("CATEGORIES")) {
			event->setProperty(key, value.split(" " || ",", QString::SkipEmptyParts));
		} else {
			event->setProperty(key, value);
		}*/
	}
	m_eventList.append(QVariant::fromValue(event));
}
