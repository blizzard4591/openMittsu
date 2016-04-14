#ifndef OPENMITTSU_UPDATER_UPDATER_H_
#define OPENMITTSU_UPDATER_UPDATER_H_

#include <QObject>
#include <QThread>

class Updater : public QThread {
	Q_OBJECT
public:
	Updater();
	~Updater();
signals:
	void foundNewVersion(int major, int minor, int patch, int commitsSinceTag, QString gitHash, QString channel, QString link);
protected:
	virtual void run() override;
};

#endif // OPENMITTSU_UPDATER_UPDATER_H_