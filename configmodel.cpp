#include "configmodel.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

ConfigModel::ConfigModel(QFile *configFile, QFile *symptomsFile, QObject *parent) :
	QObject(parent),
	_weightUnit("kg"),
	_temperatureUnit("c"),
	_dataFilePath(QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)).absoluteFilePath("FemmeJournal.ics")),
	_allSymptoms({
		"acne",
		"angry",
		"anxious",
		"backaches",
		"bloating",
		"bodyaches",
		"calm",
		"confident",
		"confused",
		"constipation",
		"cramps",
		"cravings (salty)",
		"cravings (sweets)",
		"depressed",
		"dizziness",
		"energized",
		"fatigue",
		"fatigued",
		"flirty",
		"forgetful",
		"frustrated",
		"gloomy",
		"grumpy",
		"happy",
		"headaches",
		"hungry",
		"hyper",
		"impatient",
		"in love",
		"insecure",
		"insomnia",
		"irritable",
		"jealous",
		"lethargic",
		"mean",
		"moody",
		"nausea",
		"neckaches",
		"nervous",
		"refreshed",
		"sad",
		"scared",
		"sensitive",
		"shy",
		"sick",
		"spacey",
		"spotting",
		"stressed",
		"tender breasts",
		"unbalanced",
		"weepy"
	}),
	_symptomsFile(symptomsFile),
	_configFile(configFile) {
	Q_ASSERT(symptomsFile);
	Q_ASSERT(configFile);
	_symptomsStream.setDevice(_symptomsFile);

	QString line = _symptomsStream.readLine();
	while(!line.isNull()) {
		addSymptom(line, false);
		line = _symptomsStream.readLine();
	}

	_configFile->seek(0);
	QVariantMap fromFile(QJsonDocument::fromJson(_configFile->readAll()).object().toVariantMap());
	if(fromFile.contains("weightUnit")) _weightUnit = fromFile["weightUnit"].toString().toCaseFolded();
	if(fromFile.contains("temperatureUnit")) _temperatureUnit = fromFile["temperatureUnit"].toString().toCaseFolded();
	if(fromFile.contains("dataFilePath")) _dataFilePath = fromFile["dataFilePath"].toString();

	connect(this, SIGNAL(weightUnitChanged(QString,QString)), this, SLOT(saveConfig()));
	connect(this, SIGNAL(temperatureUnitChanged(QString,QString)), this, SLOT(saveConfig()));
	connect(this, SIGNAL(dataFilePathChanged(QString)), this, SLOT(saveConfig()));

	saveSymptoms();
}

ConfigModel::~ConfigModel() {
	_symptomsFile->close();
	_configFile->close();
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

void ConfigModel::setWeightUnit(QString unit) {
	if(unit == _weightUnit) return;
	QString oldUnit = _weightUnit;
	_weightUnit = unit;
	emit weightUnitChanged(oldUnit, unit);
}

void ConfigModel::setTemperatureUnit(QString unit) {
	if(unit == _temperatureUnit) return;
	QString oldUnit = _temperatureUnit;
	_temperatureUnit = unit;
	emit temperatureUnitChanged(oldUnit, unit);
}

QFileInfo ConfigModel::dataFileInfo() {
	return QFileInfo(_dataFilePath);
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

void ConfigModel::saveConfig() {
	_configFile->seek(0);
	_configFile->resize(0);

	QJsonObject o;
	o["weightUnit"] = _weightUnit;
	o["temperatureUnit"] = _temperatureUnit;
	o["dataFilePath"] = _dataFilePath;

	_configFile->write(QJsonDocument(o).toJson());
}
