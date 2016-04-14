#ifndef OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_

#include "tasks/CallbackTask.h"

#include <QList>
#include <QString>
#include <QSslCertificate>
#include <QSslConfiguration>


class CertificateBasedCallbackTask : public virtual CallbackTask {
public:
	virtual ~CertificateBasedCallbackTask();
protected:
	CertificateBasedCallbackTask(QList<QSslCertificate> const& acceptableNonCaRootCertificates);
	CertificateBasedCallbackTask(QString const& acceptableNonCaRootCertificateInBase64);

	virtual void preRunSetup() override;

	QSslConfiguration getSslConfigurationWithCaCerts();
private:
	void setupNetworkFacilities();
	CertificateBasedCallbackTask();

	QList<QSslCertificate> acceptableNonCaRootCertificates;
};

#endif // OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_