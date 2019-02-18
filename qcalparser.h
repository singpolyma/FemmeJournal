// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#ifndef QCALPARSER_H
#define QCALPARSER_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QSaveFile>
#include <QTimer>
#include <QDate>

#include "configmodel.h"
#include "journalentry.h"

class QCalParser : public QObject {
	Q_OBJECT
public:
	QCalParser(QString path, ConfigModel *config, QObject *parent = 0);

public slots:
	void parse();
	void changeDataFilePath(QString newPath);
	void changeWeightUnit(QString oldUnit, QString newUnit);
	void changeTemperatureUnit(QString oldUnit, QString newUnit);
	void updateJournalLines(QDate date, QStringList icsLines);
	void save();

signals:
	void doneParse();
	void newJournalEntry(QDate date, JournalEntry *entry);

protected:
	void parseBlock(QString weightUnit, QString temperatureUnit, QTextStream *dataStream);

	QList<QVariant> m_eventList;
	QString _path;
	QTimer _timer;
	ConfigModel *_config;
	QString _weightUnit;
	QString _temperatureUnit;
};

#endif // QCALPARSER_H
