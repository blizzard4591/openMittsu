#ifndef OPENMITTSU_NETWORK_SERVERCONFIGURATION_H_
#define OPENMITTSU_NETWORK_SERVERCONFIGURATION_H_

#include "src/protocol/ProtocolSpecs.h"
#include "src/crypto/PublicKey.h"
#include <QString>
#include <cstdint>

namespace openmittsu {
	namespace network {

		class ServerConfiguration {
		public:
			ServerConfiguration();
			ServerConfiguration(QString const& serverHost, int serverPort, openmittsu::crypto::PublicKey const& serverLongTermPublicKey, QString const& apiServerHost, QString const& apiServerAgent, QString const& apiServerCertificate, QString const& blobServerRequestDownloadUrl, QString const& blobServerRequestDownloadFinishedUrl, QString const& blobServerRequestUploadUrl, QString const& blobServerRequestAgent, QString const& blobServerCertificate);

			QString const& getServerHost() const;
			int getServerPort() const;
			openmittsu::crypto::PublicKey const& getServerLongTermPublicKey() const;

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

			QString const& getSafeServerHost() const;
			QString const& getSafeServerAgent() const;
			QString const& getSafeServerCertificateAsBase64() const;
			QString getSafeServerConfigUrl() const;
			QString getSafeServerBackupUrl() const;
			
			QString const& getBlobServerRequestDownloadUrl() const;
			QString const& getBlobServerRequestDownloadFinishedUrl() const;
			QString const& getBlobServerRequestUploadUrl() const;
			QString const& getBlobServerRequestAgent() const;
			QString const& getBlobServerCertificateAsBase64() const;

			QString toString() const;
		private:
			QString const serverHost;
			int const serverPort;
			openmittsu::crypto::PublicKey const serverLongTermPublicKey;

			QString const apiServerHost;
			QString const apiServerAgent;
			QString const apiServerCertificate;

			QString const blobServerRequestDownloadUrl;
			QString const blobServerRequestDownloadFinishedUrl;
			QString const blobServerRequestUploadUrl;
			QString const blobServerRequestAgent;
			QString const blobServerCertificate;

			QString const safeServerHost;
			QString const safeServerAgent;
			QString const safeServerCertificate;

			QString const& getApiServerHost() const;
		};


	}
}

#endif // OPENMITTSU_NETWORK_SERVERCONFIGURATION_H_