#include "journalentry.h"

#include <QDebug>

JournalEntry::JournalEntry(QObject *parent) :
	QObject(parent),
	_menstrualChange(MenstruationUnchanged),
	_intimate(false),
	_ovulated(false),
	_note("") {
	connect(this, SIGNAL(intimateChanged()), this, SIGNAL(emptyChanged()));
	connect(this, SIGNAL(noteChanged()), this, SIGNAL(emptyChanged()));

	connect(this, SIGNAL(intimateChanged()), this, SIGNAL(changed()));
	connect(this, SIGNAL(ovulatedChanged()), this, SIGNAL(changed()));
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

void JournalEntry::addUnknownLine(QString line) {
	_unknown.append(line);
	emit changed();
}

bool JournalEntry::empty() {
	return !_intimate && _note == "";
}

void JournalEntry::readProperty(QByteArray name, void *ret) {
	*((QVariant*)ret) = property(name.constData());
}
