#include "configmodel.h"

#include <QDebug>

ConfigModel::ConfigModel(QObject *parent) :
	QObject(parent),
	_allSymptoms({"acne"}) {
}

const QStringList& ConfigModel::allSymptoms() const {
	return _allSymptoms;
}

int ConfigModel::addSymptom(QString symptom) {
	_allSymptoms.append(symptom);
	_allSymptoms.sort();
	return _allSymptoms.count();
}
