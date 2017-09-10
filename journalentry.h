#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H

#include <QtCore/QObject>
#include <QtCore/qabstractitemmodel.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class JournalEntry : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool menstruationStarted READ menstruationStarted WRITE setMenstruationStarted NOTIFY menstruationStartedChanged FINAL)
	Q_PROPERTY(bool menstruationStopped READ menstruationStopped WRITE setMenstruationStopped NOTIFY menstruationStoppedChanged FINAL)
	Q_PROPERTY(bool intimate MEMBER _intimate NOTIFY intimateChanged FINAL)
	Q_PROPERTY(bool ovulated MEMBER _ovulated NOTIFY ovulatedChanged FINAL)
	Q_PROPERTY(QString note MEMBER _note NOTIFY noteChanged FINAL)
	Q_PROPERTY(bool empty READ empty NOTIFY emptyChanged FINAL)

public:
	explicit JournalEntry(QObject *parent = nullptr);

	bool menstruationStarted();
	void setMenstruationStarted(bool started);

	bool menstruationStopped();
	void setMenstruationStopped(bool stopped);

	bool empty();

	enum MenstrualChange {
		MenstruationUnchanged,
		MenstruationStarted,
		MenstruationStopped
	};

signals:
	void menstruationStartedChanged();
	void menstruationStoppedChanged();
	void intimateChanged();
	void ovulatedChanged();
	void noteChanged();
	void emptyChanged();

protected:
	enum MenstrualChange _menstrualChange;
	bool _intimate;
	bool _ovulated;
	QString _note;

	Q_DISABLE_COPY(JournalEntry)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(JournalEntry)

#endif // JOURNALENTRY_H
