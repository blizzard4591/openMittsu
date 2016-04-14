#include "CertificateBasedCallbackTask.h"

#include "exceptions/IllegalArgumentException.h"
#include "utility/Logging.h"

#include <QSslError>
#include <QNetworkReply>

CertificateBasedCallbackTask::CertificateBasedCallbackTask() : CallbackTask() {
	throw;
}

CertificateBasedCallbackTask::~CertificateBasedCallbackTask() {
	// Intentionally left empty.
}

CertificateBasedCallbackTask::CertificateBasedCallbackTask(QList<QSslCertificate> const & acceptableNonCaRootCertificates) : CallbackTask(), acceptableNonCaRootCertificates(acceptableNonCaRootCertificates) {
	// Intentionally left empty.
}

void CertificateBasedCallbackTask::preRunSetup() {
	setupNetworkFacilities();
}

QSslConfiguration CertificateBasedCallbackTask::getSslConfigurationWithCaCerts() {
	QSslConfiguration defaultConfig = QSslConfiguration::defaultConfiguration();
	QList<QSslCertificate> defaultCerts = defaultConfig.caCertificates();
	defaultCerts.append(acceptableNonCaRootCertificates);
	defaultConfig.setCaCertificates(defaultCerts);

	return defaultConfig;
}

CertificateBasedCallbackTask::CertificateBasedCallbackTask(QString const & acceptableNonCaRootCertificateInBase64) : CallbackTask(), acceptableNonCaRootCertificates() {
	QByteArray certData = QByteArray::fromBase64(acceptableNonCaRootCertificateInBase64.toLocal8Bit());
	QSslCertificate acceptableNonCaRootCertificate = QSslCertificate(certData, QSsl::EncodingFormat::Pem);
	if (acceptableNonCaRootCertificate.isNull()) {
		throw IllegalArgumentException() << "Could not convert the given server certificate " << acceptableNonCaRootCertificateInBase64.toStdString() << " to a valid SSL certificate.";
	}

	acceptableNonCaRootCertificates.append(acceptableNonCaRootCertificate);
}

void CertificateBasedCallbackTask::setupNetworkFacilities() {
	//
}
