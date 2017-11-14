#ifndef OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_
#define OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_

#include <QList>
#include <QString>
#include <QSslCertificate>
#include <QSslConfiguration>

namespace openmittsu {
	namespace tasks {

		class CertificateBasedCallbackTask {
		public:
			virtual ~CertificateBasedCallbackTask();
		protected:
			CertificateBasedCallbackTask(QList<QSslCertificate> const& acceptableNonCaRootCertificates);
			CertificateBasedCallbackTask(QString const& acceptableNonCaRootCertificateInBase64);

			QSslConfiguration getSslConfigurationWithCaCerts();
		private:
			CertificateBasedCallbackTask();

			QList<QSslCertificate> m_acceptableNonCaRootCertificates;
		};

	}
}

#endif // OPENMITTSU_TASKS_CERTIFICATEBASEDCALLBACKTASK_H_
