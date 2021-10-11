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
	apiServerCertificate(QStringLiteral("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUVQakNDQXlhZ0F3SUJBZ0lFU2xPTUtEQU5CZ2txaGtpRzl3MEJBUXNGQURDQnZqRUxNQWtHQTFVRUJoTUMKVlZNeEZqQVVCZ05WQkFvVERVVnVkSEoxYzNRc0lFbHVZeTR4S0RBbUJnTlZCQXNUSDFObFpTQjNkM2N1Wlc1MApjblZ6ZEM1dVpYUXZiR1ZuWVd3dGRHVnliWE14T1RBM0JnTlZCQXNUTUNoaktTQXlNREE1SUVWdWRISjFjM1FzCklFbHVZeTRnTFNCbWIzSWdZWFYwYUc5eWFYcGxaQ0IxYzJVZ2IyNXNlVEV5TURBR0ExVUVBeE1wUlc1MGNuVnoKZENCU2IyOTBJRU5sY25ScFptbGpZWFJwYjI0Z1FYVjBhRzl5YVhSNUlDMGdSekl3SGhjTk1Ea3dOekEzTVRjeQpOVFUwV2hjTk16QXhNakEzTVRjMU5UVTBXakNCdmpFTE1Ba0dBMVVFQmhNQ1ZWTXhGakFVQmdOVkJBb1REVVZ1CmRISjFjM1FzSUVsdVl5NHhLREFtQmdOVkJBc1RIMU5sWlNCM2QzY3VaVzUwY25WemRDNXVaWFF2YkdWbllXd3QKZEdWeWJYTXhPVEEzQmdOVkJBc1RNQ2hqS1NBeU1EQTVJRVZ1ZEhKMWMzUXNJRWx1WXk0Z0xTQm1iM0lnWVhWMAphRzl5YVhwbFpDQjFjMlVnYjI1c2VURXlNREFHQTFVRUF4TXBSVzUwY25WemRDQlNiMjkwSUVObGNuUnBabWxqCllYUnBiMjRnUVhWMGFHOXlhWFI1SUMwZ1J6SXdnZ0VpTUEwR0NTcUdTSWIzRFFFQkFRVUFBNElCRHdBd2dnRUsKQW9JQkFRQzZoTFp5MjU0TWErS1o2VEFCcDNicU1yaVZRUnJKMm1GT1dITFAvdmFDZWI5ellRWUtwU2ZZczEvVApSVTRjY3RaT012SnlpZy8zZ3huUWFvQ0FBRVVlc01mbm1yOFNWeWNjbzJndkNvZTlhbXNPWG1YekhIZlYxSVdOCmNDRzBzekxuaTZMVmhqa0NzYmpTUjg3a3lVbkVPNmZlKzFSOVY3N3c2RzdDZWJJNkMxWGlVSmdXTWhOY0wzaFcKd2NLVXMvSmE1Q2VhbnlUWHh1elFteVdDNDh6Q3hFWEZqSmQ2Qm1zcUVaK3BDbTVJTzIvYjFCRVpRdmVQQjcvMQpVMStjUHZRWExPWnByRTR5VEdKMzZyZm81YnMwdkJtTHJweFI1N2QrdFZPeE15TGxiYzl3UEJyNjRwdG50b1AwCmphV3ZZa3hONEZpc1pEUVNBL2kyalpSakpLUnhBZ01CQUFHalFqQkFNQTRHQTFVZER3RUIvd1FFQXdJQkJqQVAKQmdOVkhSTUJBZjhFQlRBREFRSC9NQjBHQTFVZERnUVdCQlJxY2laNjBCN3ZmZWM3YVZIVWJJMmZrQkptcXpBTgpCZ2txaGtpRzl3MEJBUXNGQUFPQ0FRRUFlWjhkbHNhMmVUOGlqWWZUaHdNRVlHcHJtaTVaaVhNUnJFUFI5UlAvCmpUa3J3UEs5VDNDTXFTL3FGOFFMVko3VUc1YVlNenlvcldLaUFIYXJXV2x1QmgxK3hMbEVqWml2RXRSaDJ3b1oKUmtmejYvZGp3VUFGUUtYU3QvUzFtamEvcVloMmlBUlZCQ3VjaDM4YU56eCtMYVVhMk5TSlhzcTlyRDFzMkcydgoxZk4yRDgwN2lEZ2luV3lUbXNROXY0SWJaVCttRDEycS9PV3lGY3ExcmNhOFBkQ0U2T29HY3JCTk9USjR2ejRSCm5BdWtuWm9oOC9DYkN6QjQyOEhjaDBQK3ZHT2F5c1hDSE1uSGpmODdFbGdJNXJZOTdIb3NUdnVEbHM0TVBHbUgKVkhPa2M4S1QvMUVRckJWVUFkajhCYkdKb1g5MGc1cEoxOXhPZTRwSWI0dEY5Zz09Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0K")),
	blobServerRequestDownloadUrl(QStringLiteral("https://blobp-%1.threema.ch/%2")),
	blobServerRequestDownloadFinishedUrl(QStringLiteral("https://blobp-%1.threema.ch/%2/done")),
	blobServerRequestUploadUrl(QStringLiteral("https://blobp-upload.threema.ch/upload")),
	blobServerRequestAgent(QStringLiteral("Threema/openMittsu")),
	blobServerCertificate(QStringLiteral("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUVQakNDQXlhZ0F3SUJBZ0lFU2xPTUtEQU5CZ2txaGtpRzl3MEJBUXNGQURDQnZqRUxNQWtHQTFVRUJoTUMKVlZNeEZqQVVCZ05WQkFvVERVVnVkSEoxYzNRc0lFbHVZeTR4S0RBbUJnTlZCQXNUSDFObFpTQjNkM2N1Wlc1MApjblZ6ZEM1dVpYUXZiR1ZuWVd3dGRHVnliWE14T1RBM0JnTlZCQXNUTUNoaktTQXlNREE1SUVWdWRISjFjM1FzCklFbHVZeTRnTFNCbWIzSWdZWFYwYUc5eWFYcGxaQ0IxYzJVZ2IyNXNlVEV5TURBR0ExVUVBeE1wUlc1MGNuVnoKZENCU2IyOTBJRU5sY25ScFptbGpZWFJwYjI0Z1FYVjBhRzl5YVhSNUlDMGdSekl3SGhjTk1Ea3dOekEzTVRjeQpOVFUwV2hjTk16QXhNakEzTVRjMU5UVTBXakNCdmpFTE1Ba0dBMVVFQmhNQ1ZWTXhGakFVQmdOVkJBb1REVVZ1CmRISjFjM1FzSUVsdVl5NHhLREFtQmdOVkJBc1RIMU5sWlNCM2QzY3VaVzUwY25WemRDNXVaWFF2YkdWbllXd3QKZEdWeWJYTXhPVEEzQmdOVkJBc1RNQ2hqS1NBeU1EQTVJRVZ1ZEhKMWMzUXNJRWx1WXk0Z0xTQm1iM0lnWVhWMAphRzl5YVhwbFpDQjFjMlVnYjI1c2VURXlNREFHQTFVRUF4TXBSVzUwY25WemRDQlNiMjkwSUVObGNuUnBabWxqCllYUnBiMjRnUVhWMGFHOXlhWFI1SUMwZ1J6SXdnZ0VpTUEwR0NTcUdTSWIzRFFFQkFRVUFBNElCRHdBd2dnRUsKQW9JQkFRQzZoTFp5MjU0TWErS1o2VEFCcDNicU1yaVZRUnJKMm1GT1dITFAvdmFDZWI5ellRWUtwU2ZZczEvVApSVTRjY3RaT012SnlpZy8zZ3huUWFvQ0FBRVVlc01mbm1yOFNWeWNjbzJndkNvZTlhbXNPWG1YekhIZlYxSVdOCmNDRzBzekxuaTZMVmhqa0NzYmpTUjg3a3lVbkVPNmZlKzFSOVY3N3c2RzdDZWJJNkMxWGlVSmdXTWhOY0wzaFcKd2NLVXMvSmE1Q2VhbnlUWHh1elFteVdDNDh6Q3hFWEZqSmQ2Qm1zcUVaK3BDbTVJTzIvYjFCRVpRdmVQQjcvMQpVMStjUHZRWExPWnByRTR5VEdKMzZyZm81YnMwdkJtTHJweFI1N2QrdFZPeE15TGxiYzl3UEJyNjRwdG50b1AwCmphV3ZZa3hONEZpc1pEUVNBL2kyalpSakpLUnhBZ01CQUFHalFqQkFNQTRHQTFVZER3RUIvd1FFQXdJQkJqQVAKQmdOVkhSTUJBZjhFQlRBREFRSC9NQjBHQTFVZERnUVdCQlJxY2laNjBCN3ZmZWM3YVZIVWJJMmZrQkptcXpBTgpCZ2txaGtpRzl3MEJBUXNGQUFPQ0FRRUFlWjhkbHNhMmVUOGlqWWZUaHdNRVlHcHJtaTVaaVhNUnJFUFI5UlAvCmpUa3J3UEs5VDNDTXFTL3FGOFFMVko3VUc1YVlNenlvcldLaUFIYXJXV2x1QmgxK3hMbEVqWml2RXRSaDJ3b1oKUmtmejYvZGp3VUFGUUtYU3QvUzFtamEvcVloMmlBUlZCQ3VjaDM4YU56eCtMYVVhMk5TSlhzcTlyRDFzMkcydgoxZk4yRDgwN2lEZ2luV3lUbXNROXY0SWJaVCttRDEycS9PV3lGY3ExcmNhOFBkQ0U2T29HY3JCTk9USjR2ejRSCm5BdWtuWm9oOC9DYkN6QjQyOEhjaDBQK3ZHT2F5c1hDSE1uSGpmODdFbGdJNXJZOTdIb3NUdnVEbHM0TVBHbUgKVkhPa2M4S1QvMUVRckJWVUFkajhCYkdKb1g5MGc1cEoxOXhPZTRwSWI0dEY5Zz09Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0K")) {
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
