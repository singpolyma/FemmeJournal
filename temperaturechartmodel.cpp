#include <QDebug>

#include "temperaturechartmodel.h"

TemperatureChartModel::TemperatureChartModel(CalendarModel *calendarModel, QObject *parent) : QAbstractTableModel(parent), _calendarModel(calendarModel), _end(QDate::currentDate()), _ready(false) { }

void TemperatureChartModel::ready() {
	_ready = true;
	beginResetModel();
	endResetModel();
	emit startChanged();
	emit endChanged();
}

void TemperatureChartModel::addJournalEntry(Date date, JournalEntry *entry) {
	connect(entry, &JournalEntry::temperatureChanged, this, [=]() {
		int row = rowCount() + _end.daysTo(date) - 1;
		if(row >= 0 && row < rowCount()) {
			emit dataChanged(index(row, 1), index(row, 1));
		}
	});
}

QDateTime TemperatureChartModel::start() {
	return _end.addDays(0 - rowCount() + 1).asDateTime();
}

QDateTime TemperatureChartModel::end() {
	return _end.asDateTime();
}

void TemperatureChartModel::setEnd(QDateTime end) {
	_end = end.date();
	ready();
}

QVariant TemperatureChartModel::data(const QModelIndex &index, int role) const {
	if(!_ready) return QVariant();
	if(role != Qt::DisplayRole) return QVariant();

	int rows = rowCount();
	if(!index.isValid() || index.row() >= rows || index.column() > columnCount()) return QVariant();

	Date date = _end.addDays(index.row() - rows + 1);
	double temperature = _calendarModel->entryOf(date)->property("temperature").toDouble();
	if(index.column() == 0) {
		return QVariant::fromValue(date.asDateTime());
	} else {
		return temperature;
	}
}

int TemperatureChartModel::rowCount(const QModelIndex &parent) const {
	if(!_ready) return 0;
	if(parent.isValid()) return 0;

	int rows = 0;
	for(Date d = _end; rows < 42 && !_calendarModel->entryOf(d)->menstruationStarted(); d = d.addDays(-1)) {
		rows++;
	}

	return rows;
}

int TemperatureChartModel::columnCount(const QModelIndex &parent) const {
	if(parent.isValid()) return 0;
	return 2;
}
