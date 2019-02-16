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

void TemperatureChartModel::addJournalEntry(QDate date, JournalEntry *entry) {
	connect(entry, &JournalEntry::temperatureChanged, this, [=]() {
		int row = rowCount() + _end.daysTo(date) - 1;
		if(row >= 0 && row < rowCount()) {
			emit dataChanged(index(row, 1), index(row, 1));
		}
	});
}

QDate TemperatureChartModel::start() {
	return _end.addDays(0 - rowCount() + 1);
}

void TemperatureChartModel::setEnd(QDate end) {
	_end = end;
	ready();
}

QVariant TemperatureChartModel::data(const QModelIndex &index, int role) const {
	if(!_ready) return QVariant();
	if(role != Qt::DisplayRole) return QVariant();

	int rows = rowCount();
	if(!index.isValid() || index.row() >= rows || index.column() > columnCount()) return QVariant();

	QDate date = _end.addDays(index.row() - rows + 1);
	double temperature = _calendarModel->entryOf(date)->property("temperature").toDouble();
	if(index.column() == 0) {
		return date;
	} else {
		return temperature;
	}
}

int TemperatureChartModel::rowCount(const QModelIndex &parent) const {
	if(!_ready) return 0;
	if(parent.isValid()) return 0;

	int rows = 0;
	for(QDate d = _end; rows < 42 && !_calendarModel->entryOf(d)->menstruationStarted(); d.addDays(-1)) {
		rows++;
	}

	return rows;
}

int TemperatureChartModel::columnCount(const QModelIndex &parent) const {
	if(parent.isValid()) return 0;
	return 2;
}
