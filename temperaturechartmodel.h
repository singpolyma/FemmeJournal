#ifndef TEMPERATURECHARTMODEL_H
#define TEMPERATURECHARTMODEL_H

#include <QAbstractTableModel>
#include <QtCore/qdatetime.h>

#include "calendarmodel.h"

QT_BEGIN_NAMESPACE

class TemperatureChartModel : public QAbstractTableModel
{
	Q_OBJECT
	Q_PROPERTY(QDateTime start READ start NOTIFY startChanged STORED false FINAL)
	Q_PROPERTY(QDateTime end READ end WRITE setEnd NOTIFY endChanged FINAL)
	Q_PROPERTY(double yMin READ yMin NOTIFY yChanged STORED false FINAL)
	Q_PROPERTY(double yMax READ yMax NOTIFY yChanged STORED false FINAL)

public:
	explicit TemperatureChartModel(ConfigModel *config, CalendarModel *calendarModel, QObject *parent = nullptr);

	QDateTime start();
	QDateTime end();
	void setEnd(QDateTime);

	double yMin();
	double yMax();

	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
	void startChanged();
	void endChanged();
	void yChanged();

public slots:
	void ready();
	void addJournalEntry(Date, JournalEntry*);

protected:
	ConfigModel *_config;
	CalendarModel *_calendarModel;
	Date _end;
	bool _ready;

	Q_DISABLE_COPY(TemperatureChartModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(TemperatureChartModel)

#endif // TEMPERATURECHARTMODEL_H
