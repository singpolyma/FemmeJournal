#include "journalentry.h"

#include <QDebug>

JournalEntry::JournalEntry(QObject *parent) :
	QObject(parent),
	_menstrualChange(MenstruationUnchanged),
	_intimate(false),
	_ovulated(false),
	_note("") { }

bool JournalEntry::menstruationStarted() {
	return _menstrualChange == MenstruationStarted;
}

void JournalEntry::setMenstruationStarted(bool began) {
	if(began && _menstrualChange == MenstruationStarted) return;
	if(!began && _menstrualChange != MenstruationStarted) return;

	_menstrualChange = began ? MenstruationStarted : MenstruationUnchanged;
	emit menstruationStartedChanged();
	emit menstruationStoppedChanged();
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
}
