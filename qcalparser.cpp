// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#include "qcalparser.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtWidgets/QApplication>
#include <QThread>

#include <QDebug>

QCalParser::QCalParser(QString path, ConfigModel *config, QObject *parent) : QObject(parent), _path(path), _timer(this), _config(config) {
	Q_ASSERT(config);
	_timer.setSingleShot(true);
	_weightUnit = config->property("weightUnit").toString();
	_temperatureUnit = config->property("temperatureUnit").toString();
	connect(config, SIGNAL(dataFilePathChanged(QString)), this, SLOT(changeDataFilePath(QString)));
	connect(config, SIGNAL(weightUnitChanged(QString,QString)), this, SLOT(changeWeightUnit(QString,QString)));
	connect(config, SIGNAL(temperatureUnitChanged(QString,QString)), this, SLOT(changeTemperatureUnit(QString,QString)));
	connect(&_timer, SIGNAL(timeout()), this, SLOT(save()));
}

void QCalParser::changeDataFilePath(QString newPath) {
	_path = newPath;
	parse();
	save();
}

void QCalParser::changeWeightUnit(QString oldUnit, QString newUnit) {
	(void)oldUnit;
	_weightUnit = newUnit;
}

void QCalParser::changeTemperatureUnit(QString oldUnit, QString newUnit) {
	(void)oldUnit;
	_temperatureUnit = newUnit;
}

void QCalParser::updateJournalLines(QDate date, QStringList icsLines) {
	for(
		QList<QVariant>::iterator i = m_eventList.begin();
		i != m_eventList.end();
		i++
	) {
		if(i->typeName() == QString("QPair<QDate,QStringList>")) {
			QPair<QDate,QStringList> v = i->value<QPair<QDate,QStringList>>();
			if(v.first == date) {
				i->setValue(QPair<QDate,QStringList>(date, icsLines));
				goto done;
			}
		}
	}
	m_eventList.append(QVariant::fromValue(QPair<QDate,QStringList>(date, icsLines)));
done:
	_timer.start(1000);
}

void QCalParser::save() {
	QString weightUnit = _weightUnit.toUpper();
	QString temperatureUnit = _temperatureUnit.toUpper();

	QSaveFile file(_path);
	file.setDirectWriteFallback(true);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(file.fileName()));
	}
	QTextStream dataStream(&file);

	for(
		QList<QVariant>::iterator i = m_eventList.begin();
		i != m_eventList.end();
		i++
	) {
		if(i->typeName() == QString("QPair<QDate,QStringList>")) {
			QPair<QDate,QStringList> v = i->value<QPair<QDate,QStringList>>();
			dataStream << "BEGIN:VJOURNAL" << endl;
			dataStream << "DTSTART:" << v.first.toString("yyyyMMdd") << endl;
			for(QStringList::iterator i = v.second.begin(); i != v.second.end(); i++) {
				dataStream << *i << endl;
			}
			dataStream << "END:VJOURNAL" << endl;
		} else {
			dataStream << i->toString() << endl;
		}
	}

	dataStream.flush();
	file.commit();
}

void QCalParser::parse() {
	QFile file(_path);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qFatal("Could not open file: %s", qUtf8Printable(file.fileName()));
	}
	QTextStream dataStream(&file);

	QString line = dataStream.readLine();
	while(!line.isNull()) {
		if(line.contains("BEGIN:VJOURNAL")) {
			parseBlock(_weightUnit, _temperatureUnit, &dataStream);
		} else {
			m_eventList.append(QVariant::fromValue(line));
		}

		line = dataStream.readLine();
	}

	file.close();
	emit doneParse();
}

void QCalParser::parseBlock(QString weightUnit, QString temperatureUnit, QTextStream *dataStream) {
	QDate date;
	JournalEntry *entry = new JournalEntry(_config);
	QString line;
	while(!(line = dataStream->readLine()).contains(QByteArray("END:VJOURNAL"))) {
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

			if(unit == "f" && temperatureUnit == "c") {
				temp -= 32;
				temp *= 5;
				temp /= 9;
				unit = "c";
			}

			if(unit == "c" && temperatureUnit == "f") {
				temp *= 9;
				temp /= 5;
				temp += 32;
				unit = "f";
			}

			if(unit == temperatureUnit) {
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
			if(unit == "lbs") unit = "lb";

			if(unit == "lb" && weightUnit == "kg") {
				weight *= 0.45359237;
				unit = "kg";
			}

			if(unit == "kg" && weightUnit == "lb") {
				weight /= 0.45359237;
				unit = "lb";
			}

			if(unit == weightUnit) {
				entry->setProperty("weight", weight);
			} else {
				entry->addUnknownLine(line);
			}
		} else {
			entry->addUnknownLine(line);
		}
	}

	if(date.isNull()) {
		QStringList icsLines = entry->icsLines();
		icsLines << "END:VJOURNAL";
		icsLines.prepend("BEGIN:VJOURNAL");
		m_eventList.append(QVariant::fromValue(icsLines.join('\n')));
		delete entry;
	} else {
		m_eventList.append(QVariant::fromValue(QPair<QDate,QStringList>(date, entry->icsLines())));
		entry->moveToThread(QApplication::instance()->thread());
		emit newJournalEntry(date, entry);
	}
}
