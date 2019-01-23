#include "journalentry.h"

#include <QDebug>

JournalEntry::JournalEntry(ConfigModel *config, QObject *parent) :
	QObject(parent),
	_menstrualChange(MenstruationUnchanged),
	_intimate(NotIntimate),
	_orgasm(OrgasmUnknown),
	_opk(OPKNone),
	_temperature(0),
	_weight(0),
	_note(""),
	_symptoms(config, this),
	_config(config) {
	Q_ASSERT(config);
	connect(config, SIGNAL(weightUnitChanged(QString,QString)), this, SLOT(changeWeightUnit(QString,QString)));
	connect(config, SIGNAL(temperatureUnitChanged(QString,QString)), this, SLOT(changeTemperatureUnit(QString,QString)));
	connect(this, SIGNAL(intimateChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(orgasmChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(opkChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(temperatureChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(weightChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(noteChanged()), this, SIGNAL(changed()));
	connect(&_symptoms, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(symptomsChanged()));
	connect(&_symptoms, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(changed()));
}

bool JournalEntry::menstruationStarted() {
	return _menstrualChange == MenstruationStarted;
}

void JournalEntry::setMenstruationStarted(bool began) {
	if(began && _menstrualChange == MenstruationStarted) return;
	if(!began && _menstrualChange != MenstruationStarted) return;

	_menstrualChange = began ? MenstruationStarted : MenstruationUnchanged;
	emit menstruationStartedChanged();
	emit menstruationStoppedChanged();
	emit changed();
}

bool JournalEntry::menstruationStopped() {
	return _menstrualChange == MenstruationStopped;
}

void JournalEntry::setMenstruationStopped(bool ended) {
	if(ended && _menstrualChange == MenstruationStopped) return;
	if(!ended && _menstrualChange != MenstruationStopped) return;

	_menstrualChange = ended ? MenstruationStopped : MenstruationUnchanged;
	emit menstruationStartedChanged();
	emit menstruationStoppedChanged();
	emit changed();
}

bool JournalEntry::intimate() {
	return _intimate != NotIntimate;
}

void JournalEntry::setIntimate(bool intimate) {
	if(this->intimate() == intimate) return;
	if(!intimate) setOrgasm(OrgasmUnknown);

	_intimate = intimate ? Intimate : NotIntimate;
	emit intimateChanged();
}

QVariant JournalEntry::intimateProtection() {
	if(_intimate == Intimate || _intimate == NotIntimate) return QVariant();
	return _intimate == IntimateProtected;
}

void JournalEntry::setIntimateProtection(QVariant protection) {
	if(protection == intimateProtection()) return;

	if(!protection.isValid() || protection.isNull()) {
		_intimate = intimate() ? Intimate : NotIntimate;
	} else {
		_intimate = protection.toBool() ? IntimateProtected : IntimateUnprotected;
	}

	emit intimateChanged();
}

void JournalEntry::setOrgasm(enum Orgasm orgasm) {
	if(orgasm == _orgasm) return;

	if(orgasm != OrgasmUnknown) setIntimate(true);
	_orgasm = orgasm;

	emit orgasmChanged();
}

SymptomsModel *JournalEntry::symptoms() {
	return &_symptoms;
}

void JournalEntry::addUnknownLine(QString line) {
	_unknown.append(line);
	emit changed();
}

void JournalEntry::changeWeightUnit(QString oldUnit, QString newUnit) {
	if(_weight == 0) return;

	if(oldUnit == "lb" && newUnit == "kg") {
		_weight *= 0.45359237;
		emit weightChanged();
	}

	if(oldUnit == "kg" && newUnit == "lb") {
		_weight /= 0.45359237;
		emit weightChanged();
	}
}

void JournalEntry::changeTemperatureUnit(QString oldUnit, QString newUnit) {
	if(_temperature == 0) return;

	if(oldUnit == "f" && newUnit == "c") {
		_temperature -= 32;
		_temperature *= 5;
		_temperature /= 9;
		emit temperatureChanged();
	}

	if(oldUnit == "c" && newUnit == "f") {
		_temperature *= 9;
		_temperature /= 5;
		_temperature += 32;
		emit temperatureChanged();
	}
}

QStringList JournalEntry::icsLines() {
	QStringList lines;

	if(menstruationStarted()) lines << "X-FEMMEJOURNAL-MENSTRUATION:STARTED";
	if(menstruationStopped()) lines << "X-FEMMEJOURNAL-MENSTRUATION:STOPPED";

	if(intimate()) {
		QString line = "X-FEMMEJOURNAL-INTIMATE";

		QVariant protection = intimateProtection();
		if(!protection.isValid() || protection.isNull()) {
			// Protection unknown
		} else if(protection.toBool()) {
			line += ";PROTECTION=TRUE";
		} else {
			line += ";PROTECTION=FALSE";
		}

		if(_orgasm == JournalEntry::HadOrgasm) {
			line += ";ORGASM=TRUE";
		} else if(_orgasm == JournalEntry::NoOrgasm) {
			line += ";ORGASM=FALSE";
		}

		lines << line + ":TRUE";
	}

	if(_opk == JournalEntry::OPKPositive) {
		lines << "X-FEMMEJOURNAL-OPK:POSITIVE";
	} else if(_opk == JournalEntry::OPKNegative) {
		lines << "X-FEMMEJOURNAL-OPK:NEGATIVE";
	}

	if(_temperature != 0) {
		lines << "X-FEMMEJOURNAL-TEMPERATURE;UNIT=" + _config->property("temperatureUnit").toString() + ":" + QString::number(_temperature);
	}

	if(_weight != 0) {
		lines << "X-FEMMEJOURNAL-WEIGHT;UNIT=" + _config->property("weightUnit").toString() + ":" + QString::number(_weight);
	}

	for(int i = 0; i < _symptoms.rowCount(); i++) {
		QVariant severity = _symptoms.data(_symptoms.index(i, 0), SymptomsModel::SeverityRole);
		if(!severity.isValid() || severity.isNull()) continue;
		QString line = "X-FEMMEJOURNAL-SYMPTOM";
		if(severity.toInt() != SymptomsModel::Unknown) line += ";SEVERITY=" + QString::number(severity.toInt());
		line += ":" + _symptoms.data(_symptoms.index(i, 0), SymptomsModel::SymptomRole).toString().toUpper();
		lines << line;
	}

	{
		QString note = _note;
		if(!note.isEmpty()) {
			note.replace("\\", "\\\\");
			note.replace("\n", "\\n");
			note.replace(",", "\\,");
			note.replace(";", "\\;");
			lines << "DESCRIPTION:" + note;
		}
	}

	for(QStringList::iterator i = _unknown.begin(); i != _unknown.end(); i++) {
		lines << *i;
	}

	return lines;
}
