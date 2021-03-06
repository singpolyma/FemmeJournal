#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H

#include <QtCore/QObject>
#include <QtCore/qabstractitemmodel.h>
#include <QtQml/qqml.h>

#include "symptomsmodel.h"

QT_BEGIN_NAMESPACE

class JournalEntry : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool menstruationStarted READ menstruationStarted WRITE setMenstruationStarted NOTIFY menstruationStartedChanged FINAL)
	Q_PROPERTY(bool menstruationStopped READ menstruationStopped WRITE setMenstruationStopped NOTIFY menstruationStoppedChanged FINAL)
	Q_PROPERTY(bool intimate READ intimate WRITE setIntimate NOTIFY intimateChanged FINAL)
	Q_PROPERTY(QVariant intimateProtection READ intimateProtection WRITE setIntimateProtection NOTIFY intimateChanged FINAL)
	Q_PROPERTY(enum Orgasm orgasm MEMBER _orgasm WRITE setOrgasm NOTIFY orgasmChanged FINAL)
	Q_PROPERTY(enum OPK opk MEMBER _opk NOTIFY opkChanged FINAL)
	Q_PROPERTY(QString note MEMBER _note NOTIFY noteChanged FINAL)
	Q_PROPERTY(double temperature MEMBER _temperature NOTIFY temperatureChanged FINAL)
	Q_PROPERTY(double weight MEMBER _weight NOTIFY weightChanged FINAL)
	Q_PROPERTY(SymptomsModel* symptoms READ symptoms NOTIFY symptomsChanged FINAL)
	Q_PROPERTY(QStringList unknownLines MEMBER _unknown FINAL)

public:
	enum Intimate {
		IntimateProtected,
		IntimateUnprotected,
		Intimate,
		NotIntimate
	};

	enum Orgasm {
		HadOrgasm,
		NoOrgasm,
		OrgasmUnknown
	};

	Q_ENUM(Orgasm)

	enum OPK {
		OPKPositive,
		OPKNegative,
		OPKNone
	};

	Q_ENUM(OPK)

	enum MenstrualChange {
		MenstruationUnchanged,
		MenstruationStarted,
		MenstruationStopped
	};

	explicit JournalEntry(ConfigModel *config = nullptr, QObject *parent = nullptr);

	bool menstruationStarted();
	void setMenstruationStarted(bool started);

	bool menstruationStopped();
	void setMenstruationStopped(bool stopped);

	bool intimate();
	void setIntimate(bool intimate);
	QVariant intimateProtection();
	void setIntimateProtection(QVariant protection);

	void setOrgasm(enum Orgasm orgasm);

	SymptomsModel *symptoms();

	void addUnknownLine(QString line);

	QStringList icsLines();

signals:
	void menstruationStartedChanged();
	void menstruationStoppedChanged();
	void intimateChanged();
	void orgasmChanged();
	void opkChanged();
	void noteChanged();
	void symptomsChanged();
	void weightChanged();
	void temperatureChanged();
	void changed();

public slots:
	void changeWeightUnit(QString oldUnit, QString newUnit);
	void changeTemperatureUnit(QString oldUnit, QString newUnit);

protected:
	enum MenstrualChange _menstrualChange;
	enum Intimate _intimate;
	enum Orgasm _orgasm;
	enum OPK _opk;
	double _temperature;
	double _weight;
	QString _note;
	QStringList _unknown;
	SymptomsModel _symptoms;
	ConfigModel *_config;

	Q_DISABLE_COPY(JournalEntry)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(JournalEntry)

#endif // JOURNALENTRY_H
