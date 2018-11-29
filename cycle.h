#ifndef CYCLE_H
#define CYCLE_H

#include <QtCore/QObject>
#include <QtCore/qdatetime.h>
#include <QtQml/qqml.h>

#include "symptomsmodel.h"

QT_BEGIN_NAMESPACE

class Cycle : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QDate date MEMBER _date CONSTANT FINAL)
	Q_PROPERTY(int length MEMBER _length CONSTANT FINAL)
	Q_PROPERTY(QVariant menstrualLength MEMBER _menstrualLength CONSTANT FINAL)
	Q_PROPERTY(QVariant ovulationDaysFromEnd READ ovulationDaysFromEnd CONSTANT FINAL)

public:
	explicit Cycle(QDate date, int length, QVariant menstrualLength, QVariant ovulationDaysFromEnd, bool opkPositive, QObject *parent = nullptr);

	QVariant ovulationDaysFromEnd(bool opkPositiveOnly = true);

protected:
	QDate _date;
	int _length;
	QVariant _menstrualLength;
	QVariant _ovulationDaysFromEnd;
	bool _opkPositive;

	Q_DISABLE_COPY(Cycle)
};

QT_END_NAMESPACE

//Q_DECLARE_METATYPE(Cycle)
QML_DECLARE_TYPE(Cycle)

#endif // CYCLE_H
