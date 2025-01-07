#include "src/network/ServerConfiguration.h"

#include "src/exceptions/IllegalArgumentException.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <QString>
#include <QFile>
#include <QSettings>

namespace openmittsu {
	namespace network {

ServerConfiguration::ServerConfiguration() : 
	serverHost(QStringLiteral("g-%1.0.threema.ch")), 
	serverPort(5222), 
	serverLongTermPublicKey(openmittsu::crypto::PublicKey::fromHexString(QStringLiteral("450b975735279fdecb3313648f5fc6ee9ff4360ea92a8c1751c661e4c0d8c909"))), 
	apiServerHost(QStringLiteral("https://apip.threema.ch")),
	apiServerAgent(QStringLiteral("Threema/openMittsu")),
	apiServerCertificate(QStringLiteral("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUNqekNDQWhXZ0F3SUJBZ0lRWEl1WnhWcVV4ZEp4VnQ3TmlZRE1KakFLQmdncWhrak9QUVFEQXpDQmlERUwKTUFrR0ExVUVCaE1DVlZNeEV6QVJCZ05WQkFnVENrNWxkeUJLWlhKelpYa3hGREFTQmdOVkJBY1RDMHBsY25ObAplU0JEYVhSNU1SNHdIQVlEVlFRS0V4VlVhR1VnVlZORlVsUlNWVk5VSUU1bGRIZHZjbXN4TGpBc0JnTlZCQU1UCkpWVlRSVkpVY25WemRDQkZRME1nUTJWeWRHbG1hV05oZEdsdmJpQkJkWFJvYjNKcGRIa3dIaGNOTVRBd01qQXgKTURBd01EQXdXaGNOTXpnd01URTRNak0xT1RVNVdqQ0JpREVMTUFrR0ExVUVCaE1DVlZNeEV6QVJCZ05WQkFnVApDazVsZHlCS1pYSnpaWGt4RkRBU0JnTlZCQWNUQzBwbGNuTmxlU0JEYVhSNU1SNHdIQVlEVlFRS0V4VlVhR1VnClZWTkZVbFJTVlZOVUlFNWxkSGR2Y21zeExqQXNCZ05WQkFNVEpWVlRSVkpVY25WemRDQkZRME1nUTJWeWRHbG0KYVdOaGRHbHZiaUJCZFhSb2IzSnBkSGt3ZGpBUUJnY3Foa2pPUFFJQkJnVXJnUVFBSWdOaUFBUWFyRlJhcWZsbwpJK2Q2MVNSdlU4WmEyRXVyeHRXMjBlWnpjYTdkbk5ZTVlmM2JvSWtEdUFVVTdGZk83bDAvNGlHenp2ZlVpbm5nCm80TitMWmZRWWNUeG1kd2xrV09yZnpDanRIRGl4NkV6blBPL0xseFRzVit6ZlRKL2lqVGplWG1qUWpCQU1CMEcKQTFVZERnUVdCQlE2NFFtRzFNOFp3cFoyZEVsMjNPQTF4bU5qbWpBT0JnTlZIUThCQWY4RUJBTUNBUVl3RHdZRApWUjBUQVFIL0JBVXdBd0VCL3pBS0JnZ3Foa2pPUFFRREF3Tm9BREJsQWpBMlo2RVdDTnprbHdCQkhVNis0V01CCnp6dXFRaEZrb0oyVU9RSVJlVng3SGZwa3VlNFdRck8vaXNJSnhPemtzVTBDTVFEcEttRkhqRkpLUzA0WWNQYlcKUk5adTlZTzZiVmk5Sk5sV1NPcnZ4S0pHZ1locU9rYlJxWnROeVdIYTBWMVhhaGc9Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0K")),
	blobServerRequestDownloadUrl(QStringLiteral("https://blobp-%1.threema.ch/%2")),
	blobServerRequestDownloadFinishedUrl(QStringLiteral("https://blobp-%1.threema.ch/%2/done")),
	blobServerRequestUploadUrl(QStringLiteral("https://blobp-upload.threema.ch/upload")),
	blobServerRequestAgent(QStringLiteral("Threema/openMittsu")),
	blobServerCertificate(QStringLiteral("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUNqekNDQWhXZ0F3SUJBZ0lRWEl1WnhWcVV4ZEp4VnQ3TmlZRE1KakFLQmdncWhrak9QUVFEQXpDQmlERUwKTUFrR0ExVUVCaE1DVlZNeEV6QVJCZ05WQkFnVENrNWxkeUJLWlhKelpYa3hGREFTQmdOVkJBY1RDMHBsY25ObAplU0JEYVhSNU1SNHdIQVlEVlFRS0V4VlVhR1VnVlZORlVsUlNWVk5VSUU1bGRIZHZjbXN4TGpBc0JnTlZCQU1UCkpWVlRSVkpVY25WemRDQkZRME1nUTJWeWRHbG1hV05oZEdsdmJpQkJkWFJvYjNKcGRIa3dIaGNOTVRBd01qQXgKTURBd01EQXdXaGNOTXpnd01URTRNak0xT1RVNVdqQ0JpREVMTUFrR0ExVUVCaE1DVlZNeEV6QVJCZ05WQkFnVApDazVsZHlCS1pYSnpaWGt4RkRBU0JnTlZCQWNUQzBwbGNuTmxlU0JEYVhSNU1SNHdIQVlEVlFRS0V4VlVhR1VnClZWTkZVbFJTVlZOVUlFNWxkSGR2Y21zeExqQXNCZ05WQkFNVEpWVlRSVkpVY25WemRDQkZRME1nUTJWeWRHbG0KYVdOaGRHbHZiaUJCZFhSb2IzSnBkSGt3ZGpBUUJnY3Foa2pPUFFJQkJnVXJnUVFBSWdOaUFBUWFyRlJhcWZsbwpJK2Q2MVNSdlU4WmEyRXVyeHRXMjBlWnpjYTdkbk5ZTVlmM2JvSWtEdUFVVTdGZk83bDAvNGlHenp2ZlVpbm5nCm80TitMWmZRWWNUeG1kd2xrV09yZnpDanRIRGl4NkV6blBPL0xseFRzVit6ZlRKL2lqVGplWG1qUWpCQU1CMEcKQTFVZERnUVdCQlE2NFFtRzFNOFp3cFoyZEVsMjNPQTF4bU5qbWpBT0JnTlZIUThCQWY4RUJBTUNBUVl3RHdZRApWUjBUQVFIL0JBVXdBd0VCL3pBS0JnZ3Foa2pPUFFRREF3Tm9BREJsQWpBMlo2RVdDTnprbHdCQkhVNis0V01CCnp6dXFRaEZrb0oyVU9RSVJlVng3SGZwa3VlNFdRck8vaXNJSnhPemtzVTBDTVFEcEttRkhqRkpLUzA0WWNQYlcKUk5adTlZTzZiVmk5Sk5sV1NPcnZ4S0pHZ1locU9rYlJxWnROeVdIYTBWMVhhaGc9Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0K")) {
	//
}

ServerConfiguration::ServerConfiguration(QString const& serverHost, int serverPort, openmittsu::crypto::PublicKey const& serverLongTermPublicKey, QString const& apiServerHost, QString const& apiServerAgent, QString const& apiServerCertificate, QString const& blobServerRequestDownloadUrl, QString const& blobServerRequestDownloadFinishedUrl, QString const& blobServerRequestUploadUrl, QString const& blobServerRequestAgent, QString const& blobServerCertificate)
	: serverHost(serverHost), serverPort(serverPort), serverLongTermPublicKey(serverLongTermPublicKey), apiServerHost(apiServerHost), apiServerAgent(apiServerAgent), apiServerCertificate(apiServerCertificate), blobServerRequestDownloadUrl(blobServerRequestDownloadUrl), blobServerRequestDownloadFinishedUrl(blobServerRequestDownloadFinishedUrl), blobServerRequestUploadUrl(blobServerRequestUploadUrl), blobServerRequestAgent(blobServerRequestAgent), blobServerCertificate(blobServerCertificate) {
	//
}

QString const& ServerConfiguration::getServerHost() const {
	return serverHost;
}

int ServerConfiguration::getServerPort() const {
	return serverPort;
}

openmittsu::crypto::PublicKey const& ServerConfiguration::getServerLongTermPublicKey() const {
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
	QString result = QStringLiteral("ServerConfiguration(serverHost = %1, serverPort = %2, serverLongTermPublicKey = %3, apiServerHost = %4, apiServerAgent = %5, apiServerCertificate = %6, imageRequestDownloadUrl = %7, imageRequestDownloadFinishedUrl = %8, imageRequestUploadUrl = %9, imageRequestAgent = %10, imageServerCertificate = %11)")
		.arg(this->getServerHost())
		.arg(QString::number(this->getServerPort()))
		.arg(this->getServerLongTermPublicKey().toQString())
		.arg(this->getApiServerHost())
		.arg(this->getApiServerAgent())
		.arg(this->getApiServerCertificateAsBase64())
		.arg(this->getBlobServerRequestDownloadUrl())
		.arg(this->getBlobServerRequestDownloadFinishedUrl())
		.arg(this->getBlobServerRequestUploadUrl())
		.arg(this->getBlobServerRequestAgent())
		.arg(this->getBlobServerCertificateAsBase64());
	return result;
}


	}
}
