#include <QDebug>

#include "symptomsmodel.h"

QT_BEGIN_NAMESPACE

SymptomsModel::SymptomsModel(ConfigModel *config, QObject *parent) : QAbstractListModel(parent), _config(config) {
	Q_ASSERT(config);
}

void SymptomsModel::readSymptoms(void *ret) {
	*((QVariant*)ret) = QVariant::fromValue(_symptoms);
}

void SymptomsModel::setSymptomSeverity(QString symptom, enum SymptomSeverity severity) {
	_symptoms.insert(symptom, severity);

	int idx = _config->allSymptoms().indexOf(symptom);
	if(idx == -1) {
		emit dataChanged(index(0, 0), index(_config->addSymptom(symptom), 0));
	} else {
		emit dataChanged(index(idx, 0), index(idx, 0));
	}
}

bool SymptomsModel::setData(const int idx, const QVariant &value, int role) {
	return setData(index(idx, 0), value, role);
}

bool SymptomsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && index.row() < rowCount()) {
		QString symptom = _config->allSymptoms().at(index.row());
		switch (role) {
		case SeverityRole:
			if(value.isValid() && !value.isNull()) {
				if(_symptoms.contains(symptom) && _symptoms.value(symptom) == value.toInt()) return false;
				_symptoms.insert(symptom, (SymptomSeverity)value.toInt());
			} else {
				if(!_symptoms.contains(symptom)) return false;
				_symptoms.remove(symptom);
			}

			emit dataChanged(index, index, {role});
			return true;
		default:
			break;
		}
	}

	return false;
}

QVariant SymptomsModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() && index.row() < rowCount()) {
		QString symptom = _config->allSymptoms().at(index.row());
		switch (role) {
		case SymptomRole:
			return symptom;
		case SeverityRole:
			if(!_symptoms.contains(symptom)) break;
			return _symptoms.value(symptom);
		default:
			break;
		}
	}
	return QVariant();
}

int SymptomsModel::rowCount(const QModelIndex &parent) const {
	if (parent.isValid() || !_config) return 0;
	return _config->allSymptoms().count();
}

QHash<int, QByteArray> SymptomsModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[SymptomRole] = QByteArrayLiteral("symptom");
	roles[SeverityRole] = QByteArrayLiteral("severity");
	return roles;
}

QT_END_NAMESPACE
