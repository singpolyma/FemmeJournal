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

void JournalEntry::readProperty(QByteArray name, void *ret) {
	*((QVariant*)ret) = property(name.constData());
}
