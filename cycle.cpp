#include "cycle.h"

#include <QDebug>

Cycle::Cycle(Date date, int length, QVariant menstrualLength, QVariant ovulationDaysFromEnd, bool opkPositive, QObject *parent) :
	QObject(parent),
	_date(date),
	_length(length),
	_menstrualLength(menstrualLength),
	_ovulationDaysFromEnd(ovulationDaysFromEnd),
	_opkPositive(opkPositive) { }

QVariant Cycle::ovulationDaysFromEnd(bool opkPositiveOnly) {
	if(opkPositiveOnly && !_opkPositive) return QVariant();
	return _ovulationDaysFromEnd;
}
