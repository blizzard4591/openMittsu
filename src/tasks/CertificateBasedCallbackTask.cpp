#include "src/tasks/CertificateBasedCallbackTask.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/utility/Logging.h"

#include <QSslError>
#include <QNetworkReply>

namespace openmittsu {
	namespace tasks {

		CertificateBasedCallbackTask::CertificateBasedCallbackTask() {
			throw;
		}

		CertificateBasedCallbackTask::~CertificateBasedCallbackTask() {
			// Intentionally left empty.
		}

		CertificateBasedCallbackTask::CertificateBasedCallbackTask(QList<QSslCertificate> const & acceptableNonCaRootCertificates) : m_acceptableNonCaRootCertificates(acceptableNonCaRootCertificates) {
			// Intentionally left empty.
		}

		QSslConfiguration CertificateBasedCallbackTask::getSslConfigurationWithCaCerts() {
			QSslConfiguration defaultConfig = QSslConfiguration::defaultConfiguration();
			QList<QSslCertificate> defaultCerts = defaultConfig.caCertificates();
			defaultCerts.append(m_acceptableNonCaRootCertificates);
			defaultConfig.setCaCertificates(defaultCerts);

			return defaultConfig;
		}

		CertificateBasedCallbackTask::CertificateBasedCallbackTask(QString const& acceptableNonCaRootCertificateInBase64) : m_acceptableNonCaRootCertificates() {
			QByteArray certData = QByteArray::fromBase64(acceptableNonCaRootCertificateInBase64.toLocal8Bit());
			QSslCertificate acceptableNonCaRootCertificate = QSslCertificate(certData, QSsl::EncodingFormat::Pem);
			if (acceptableNonCaRootCertificate.isNull()) {
				throw openmittsu::exceptions::IllegalArgumentException() << "Could not convert the given server certificate " << acceptableNonCaRootCertificateInBase64.toStdString() << " to a valid SSL certificate.";
			}

			m_acceptableNonCaRootCertificates.append(acceptableNonCaRootCertificate);
		}

	}
}
