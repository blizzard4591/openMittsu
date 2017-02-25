#ifndef OPENMITTSU_SERVERCONFIGURATION_H_
#define OPENMITTSU_SERVERCONFIGURATION_H_

#include "protocol/ProtocolSpecs.h"
#include "PublicKey.h"
#include <QString>
#include <cstdint>

class ServerConfiguration {
public:
	ServerConfiguration();
	ServerConfiguration(QString const& serverHost, int serverPort, PublicKey const& serverLongTermPublicKey, QString const& apiServerHost, QString const& apiServerAgent, QString const& apiServerCertificate, QString const& blobServerRequestDownloadUrl, QString const& blobServerRequestDownloadFinishedUrl, QString const& blobServerRequestUploadUrl, QString const& blobServerRequestAgent, QString const& blobServerCertificate);

	QString const& getServerHost() const;
	int getServerPort() const;
	PublicKey const& getServerLongTermPublicKey() const;

	QString const& getApiServerAgent() const;
	QString const& getApiServerCertificateAsBase64() const;

	QString getApiServerFetchPublicKeyForIdUrl() const;
	QString getApiServerFetchFeatureLevelsForIdsUrl() const;
	QString getApiServerSetFeatureLevelUrl() const;
	QString getApiServerCheckStatusForIdsUrl() const;
	QString getApiServerCheckRevocationKeyUrl() const;
	QString getApiServerSetRevocationKeyUrl() const;
	QString getApiServerLinkEmailToIdUrl() const;
	QString getApiServerLinkMobileNumberToIdUrl() const;
	QString getApiServerRequestCallForVerificationIdUrl() const;
	QString getApiServerFetchPrivateInformationForIdUrl() const;
	QString getApiServerMatchMobileNumbersAndEmailsToContactsUrl() const;

	
	QString const& getBlobServerRequestDownloadUrl() const;
	QString const& getBlobServerRequestDownloadFinishedUrl() const;
	QString const& getBlobServerRequestUploadUrl() const;
	QString const& getBlobServerRequestAgent() const;
	QString const& getBlobServerCertificateAsBase64() const;

	QString toString() const;
private:
	QString const serverHost;
	int const serverPort;
	PublicKey const serverLongTermPublicKey;

	QString const apiServerHost;
	QString const apiServerAgent;
	QString const apiServerCertificate;

	QString const blobServerRequestDownloadUrl;
	QString const blobServerRequestDownloadFinishedUrl;
	QString const blobServerRequestUploadUrl;
	QString const blobServerRequestAgent;
	QString const blobServerCertificate;

	QString const& getApiServerHost() const;
};

#endif // OPENMITTSU_SERVERCONFIGURATION_H_