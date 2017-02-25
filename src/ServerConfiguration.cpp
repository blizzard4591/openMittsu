#include "ServerConfiguration.h"

#include "exceptions/IllegalArgumentException.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <QString>
#include <QStringBuilder>
#include <QFile>
#include <QSettings>

ServerConfiguration::ServerConfiguration() : serverHost(""), serverPort(-1), serverLongTermPublicKey(), apiServerHost(""), apiServerAgent(""), apiServerCertificate(""), blobServerRequestDownloadUrl(""), blobServerRequestDownloadFinishedUrl(""), blobServerRequestUploadUrl(""), blobServerRequestAgent(""), blobServerCertificate("") {
	//
}

ServerConfiguration::ServerConfiguration(QString const& serverHost, int serverPort, PublicKey const& serverLongTermPublicKey, QString const& apiServerHost, QString const& apiServerAgent, QString const& apiServerCertificate, QString const& blobServerRequestDownloadUrl, QString const& blobServerRequestDownloadFinishedUrl, QString const& blobServerRequestUploadUrl, QString const& blobServerRequestAgent, QString const& blobServerCertificate)
	: serverHost(serverHost), serverPort(serverPort), serverLongTermPublicKey(serverLongTermPublicKey), apiServerHost(apiServerHost), apiServerAgent(apiServerAgent), apiServerCertificate(apiServerCertificate), blobServerRequestDownloadUrl(blobServerRequestDownloadUrl), blobServerRequestDownloadFinishedUrl(blobServerRequestDownloadFinishedUrl), blobServerRequestUploadUrl(blobServerRequestUploadUrl), blobServerRequestAgent(blobServerRequestAgent), blobServerCertificate(blobServerCertificate) {
	//
}

QString const& ServerConfiguration::getServerHost() const {
	return serverHost;
}

int ServerConfiguration::getServerPort() const {
	return serverPort;
}

PublicKey const& ServerConfiguration::getServerLongTermPublicKey() const {
	return serverLongTermPublicKey;
}

QString const& ServerConfiguration::getApiServerHost() const {
	return apiServerHost;
}

QString const& ServerConfiguration::getApiServerAgent() const {
	return apiServerAgent;
}

QString const& ServerConfiguration::getApiServerCertificateAsBase64() const {
	return apiServerCertificate;
}

QString ServerConfiguration::getApiServerFetchPublicKeyForIdUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/%1"));
}

QString ServerConfiguration::getApiServerFetchFeatureLevelsForIdsUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/check_featurelevel"));
}

QString ServerConfiguration::getApiServerSetFeatureLevelUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/set_featurelevel"));
}

QString ServerConfiguration::getApiServerCheckStatusForIdsUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/check"));
}

QString ServerConfiguration::getApiServerCheckRevocationKeyUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/check_revocation_key"));
}

QString ServerConfiguration::getApiServerSetRevocationKeyUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/set_revocation_key"));
}

QString ServerConfiguration::getApiServerLinkEmailToIdUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/link_email"));
}

QString ServerConfiguration::getApiServerLinkMobileNumberToIdUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/link_mobileno"));
}

QString ServerConfiguration::getApiServerRequestCallForVerificationIdUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/link_mobileno_call"));
}

QString ServerConfiguration::getApiServerFetchPrivateInformationForIdUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/fetch_priv"));
}

QString ServerConfiguration::getApiServerMatchMobileNumbersAndEmailsToContactsUrl() const {
	return QString(getApiServerHost()).append(QStringLiteral("/identity/match"));
}


QString const& ServerConfiguration::getBlobServerRequestDownloadUrl() const {
	return blobServerRequestDownloadUrl;
}

QString const& ServerConfiguration::getBlobServerRequestDownloadFinishedUrl() const {
	return blobServerRequestDownloadFinishedUrl;
}

QString const& ServerConfiguration::getBlobServerRequestUploadUrl() const {
	return blobServerRequestUploadUrl;
}

QString const& ServerConfiguration::getBlobServerRequestAgent() const {
	return blobServerRequestAgent;
}

QString const& ServerConfiguration::getBlobServerCertificateAsBase64() const {
	return blobServerCertificate;
}

QString ServerConfiguration::toString() const {
	QString result = "ServerConfiguration(serverHost = " % this->getServerHost() %
		", serverPort = " % QString::number(this->getServerPort()) % 
		", serverLongTermPublicKey = " % this->getServerLongTermPublicKey().toString() % 
		", apiServerHost = " % this->getApiServerHost() % 
		", apiServerAgent = " % this->getApiServerAgent() % 
		", apiServerCertificate = " % this->getApiServerCertificateAsBase64() %
		", imageRequestDownloadUrl = " % this->getBlobServerRequestDownloadUrl() % 
		", imageRequestDownloadFinishedUrl = " % this->getBlobServerRequestDownloadFinishedUrl() % 
		", imageRequestUploadUrl = " % this->getBlobServerRequestUploadUrl() % 
		", imageRequestAgent = " % this->getBlobServerRequestAgent() % 
		", imageServerCertificate = " % this->getBlobServerCertificateAsBase64() % ")";
	return result;
}
