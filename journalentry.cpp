#include "journalentry.h"

#include <QDebug>

JournalEntry::JournalEntry(QObject *parent) :
	QObject(parent),
	_menstrualChange(MenstruationUnchanged),
	_intimate(NotIntimate),
	_orgasm(OrgasmUnknown),
	_opk(OPKNone),
	_note("") {
	connect(this, SIGNAL(intimateChanged()), this, SIGNAL(emptyChanged()));
	connect(this, SIGNAL(noteChanged()), this, SIGNAL(emptyChanged()));

	connect(this, SIGNAL(intimateChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(orgasmChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(opkChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(noteChanged()), this, SIGNAL(changed()));
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

bool JournalEntry::empty() {
	return !intimate() && _note == "";
}

void JournalEntry::readProperty(QByteArray name, void *ret) {
	*((QVariant*)ret) = property(name.constData());
}
