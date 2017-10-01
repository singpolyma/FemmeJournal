#include "configmodel.h"

#include <QDir>
#include <QStandardPaths>
#include <QDebug>

ConfigModel::ConfigModel(QFile *symptomsFile, QObject *parent) :
	QObject(parent),
	_allSymptoms({"acne"}),
	_symptomsFile(symptomsFile) {
	Q_ASSERT(symptomsFile);
	_symptomsStream.setDevice(_symptomsFile);

	QString line = _symptomsStream.readLine();
	while(!line.isNull()) {
		addSymptom(line, false);
		line = _symptomsStream.readLine();
	}

	saveSymptoms();
}

ConfigModel::~ConfigModel() {
	_symptomsFile->close();
}

const QStringList& ConfigModel::allSymptoms() const {
	return _allSymptoms;
}

int ConfigModel::addSymptom(QString symptom, bool save) {
	symptom = symptom.toCaseFolded();
	if(_allSymptoms.indexOf(symptom) == -1) {
		_allSymptoms.append(symptom);
		_allSymptoms.sort();
		if(save) saveSymptoms();
	}
	return _allSymptoms.count();
}

void ConfigModel::saveSymptoms() {
	_symptomsStream.seek(0);
	_symptomsFile->seek(0);
	_symptomsFile->resize(0);
	for(QStringList::iterator i = _allSymptoms.begin(); i != _allSymptoms.end(); i++) {
		_symptomsStream << *i << endl;
	}
	_symptomsStream.flush();
}
