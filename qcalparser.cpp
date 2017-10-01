// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#include "qcalparser.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtWidgets/QApplication>
#include <QThread>

#include <QDebug>

QCalParser::QCalParser(QFile *file, ConfigModel *config, QObject *parent) : QObject(parent), m_dataStream(), _timer(this), _config(config) {
	Q_ASSERT(config);
	_file = file;
	_timer.setSingleShot(true);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(save()));
	m_dataStream.setDevice(_file);
}

QCalParser::~QCalParser() {
	_file->close();
}

void QCalParser::addJournalEntry(QDate date, JournalEntry *entry) {
	Q_ASSERT(entry);
	m_eventList.append(QVariant::fromValue(QPair<QDate,JournalEntry*>(date, entry)));
}

void QCalParser::delaySave() {
	_timer.start(1000);
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
	if(retVal.toBool()) lines << "X-FEMMEJOURNAL-MENSTRUATION:STARTED";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "menstruationStopped"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) lines << "X-FEMMEJOURNAL-MENSTRUATION:STOPPED";

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "intimate"), Q_ARG(void*, &retVal));
	if(retVal.toBool()) {
		QString line = "X-FEMMEJOURNAL-INTIMATE";

		QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "intimateProtection"), Q_ARG(void*, &retVal));
		if(!retVal.isValid() || retVal.isNull()) {
			// Protection unknown
		} else if(retVal.toBool()) {
			line += ";PROTECTION=TRUE";
		} else {
			line += ";PROTECTION=FALSE";
		}

		QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "orgasm"), Q_ARG(void*, &retVal));
		if(retVal.toInt() == JournalEntry::HadOrgasm) {
			line += ";ORGASM=TRUE";
		} else if(retVal.toInt() == JournalEntry::NoOrgasm) {
			line += ";ORGASM=FALSE";
		}

		lines << line + ":TRUE";
	}

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "opk"), Q_ARG(void*, &retVal));
	if(retVal.toInt() == JournalEntry::OPKPositive) {
		lines << "X-FEMMEJOURNAL-OPK:POSITIVE";
	} else if(retVal.toInt() == JournalEntry::OPKNegative) {
		lines << "X-FEMMEJOURNAL-OPK:NEGATIVE";
	}

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "temperature"), Q_ARG(void*, &retVal));
	if(retVal.toDouble() != 0) {
		lines << "X-FEMMEJOURNAL-TEMPERATURE;UNIT=C:" + QString::number(retVal.toDouble());
	}

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "weight"), Q_ARG(void*, &retVal));
	if(retVal.toDouble() != 0) {
		lines << "X-FEMMEJOURNAL-WEIGHT;UNIT=KG:" + QString::number(retVal.toDouble());
	}

	{
		QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "symptoms"), Q_ARG(void*, &retVal));
		SymptomsModel *model = retVal.value<SymptomsModel*>();
		QMetaObject::invokeMethod(model, "readSymptoms", Qt::BlockingQueuedConnection, Q_ARG(void*, &retVal));
		QMap<QString, enum SymptomsModel::SymptomSeverity> symptoms = retVal.value<QMap<QString, enum SymptomsModel::SymptomSeverity>>();
		for(
			QMap<QString, enum SymptomsModel::SymptomSeverity>::const_iterator i = symptoms.cbegin();
			i != symptoms.cend();
			i++
		) {
			QString line = "X-FEMMEJOURNAL-SYMPTOM";
			if(*i != SymptomsModel::Unknown) line += ";SEVERITY=" + QString::number(*i);
			line += ":" + i.key().toUpper();
			lines << line;
		}
	}

	QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "note"), Q_ARG(void*, &retVal));
	QString note = retVal.toString();
	if(!note.isEmpty()) {
		note.replace("\\", "\\\\");
		note.replace("\n", "\\n");
		note.replace(",", "\\,");
		note.replace(";", "\\;");
		lines << "DESCRIPTION:" + note;
	}

	{
		QMetaObject::invokeMethod(entry, "readProperty", Qt::BlockingQueuedConnection, Q_ARG(QByteArray, "unknownLines"), Q_ARG(void*, &retVal));
		QStringList unknownLines = retVal.value<QStringList>();
		for(QStringList::iterator i = unknownLines.begin(); i != unknownLines.end(); i++) {
			lines << *i;
		}
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

	emit doneParse();
}

void QCalParser::parseBlock() {
	QDate date;
	JournalEntry *entry = new JournalEntry(_config);
	QString line;
	while(!(line = m_dataStream.readLine()).contains(QByteArray("END:VJOURNAL"))) {
		const int deliminatorPosition = line.indexOf(QChar(':'));
		const QString key   = line.mid(0, deliminatorPosition);
		const QString value = line.mid(deliminatorPosition + 1, -1);
		const QStringList attrs = key.split(';', QString::SkipEmptyParts);

		if(attrs.at(0) == QString("DTSTART")) {
			date = QDate::fromString(value, "yyyyMMdd");
		} else if(attrs.at(0) == QString("DESCRIPTION")) {
			QString desc = value;
			desc.replace("\\N", "\n");
			desc.replace("\\n", "\n");
			desc.replace("\\,", ",");
			desc.replace("\\;", ";");
			desc.replace("\\\\", "\\");
			entry->setProperty("note", desc);
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-MENSTRUATION") && value.toCaseFolded() == QString("started")) {
			entry->setMenstruationStarted(true);
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-MENSTRUATION") && value.toCaseFolded() == QString("stopped")) {
			entry->setMenstruationStopped(true);
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-INTIMATE") && value.toCaseFolded() == QString("true")) {
			entry->setProperty("intimate", true);

			for(
				QStringList::const_iterator i = attrs.begin();
				i != attrs.end();
				i++
			) {
				if(i->toCaseFolded() == QString("protection=true")) {
					entry->setProperty("intimateProtection", true);
				} else if(i->toCaseFolded() == QString("protection=false")) {
					entry->setProperty("intimateProtection", false);
				} else if(i->toCaseFolded() == QString("orgasm=true")) {
					entry->setProperty("orgasm", JournalEntry::HadOrgasm);
				} else if(i->toCaseFolded() == QString("orgasm=false")) {
					entry->setProperty("orgasm", JournalEntry::NoOrgasm);
				}
			}
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-OPK")) {
			if(value.toCaseFolded() == QString("positive")) {
				entry->setProperty("opk", JournalEntry::OPKPositive);
			} else if(value.toCaseFolded() == QString("negative")) {
				entry->setProperty("opk", JournalEntry::OPKNegative);
			}
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-SYMPTOM")) {
			SymptomsModel *symptoms = entry->property("symptoms").value<SymptomsModel*>();
			int severity = SymptomsModel::Unknown;

			for(
				QStringList::const_iterator i = attrs.begin();
				i != attrs.end();
				i++
			) {
				if(i->toCaseFolded().startsWith(QString("severity="))) {
					severity = i->midRef(9).toInt();
				}
			}

			symptoms->setSymptomSeverity(value.toCaseFolded(), (SymptomsModel::SymptomSeverity)severity);
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-TEMPERATURE")) {
			QString unit = "c";
			double temp = value.toDouble();

			for(
				QStringList::const_iterator i = attrs.begin();
				i != attrs.end();
				i++
			) {
				if(i->toCaseFolded().startsWith(QString("unit="))) {
					unit = i->mid(5).toCaseFolded();
				}
			}

			if(unit == "f") {
				temp -= 32;
				temp *= 5;
				temp /= 9;
				unit = "c";
			}

			if(unit == "c") {
				entry->setProperty("temperature", temp);
			} else {
				entry->addUnknownLine(line);
			}
		} else if(attrs.at(0) == QString("X-FEMMEJOURNAL-WEIGHT")) {
			QString unit = "kg";
			double weight = value.toDouble();

			for(
				QStringList::const_iterator i = attrs.begin();
				i != attrs.end();
				i++
			) {
				if(i->toCaseFolded().startsWith(QString("unit="))) {
					unit = i->mid(5).toCaseFolded();
				}
			}

			if(unit == "kgs") unit = "kg";

			if(unit == "lb" || unit == "lbs") {
				weight *= 0.45359237;
				unit = "kg";
			}

			if(unit == "kg") {
				entry->setProperty("weight", weight);
			} else {
				entry->addUnknownLine(line);
			}
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
