// This file includes code Copyright (C) Rohan Garg, Harald Sitter licensed
// under the LGPLv2.1-or-later from libqcalparser
// Modifications and new code for this project are licensed under the AGPLv3

#ifndef QCALPARSER_H
#define QCALPARSER_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "journalentry.h"

class QCalParser : public QObject {
	Q_OBJECT
public:
	QCalParser(QObject *parent = 0);
	~QCalParser();

	void populateModel(QObject *model, const char *slot);

public slots:
	bool parse(const QByteArray &data);
	bool parse(QFile *file);

protected:
	void parse();
	void parseBlock();

	QList<QVariant> m_eventList;
	QTextStream *m_dataStream;
};

#endif // QCALPARSER_H
