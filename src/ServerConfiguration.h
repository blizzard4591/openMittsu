#ifndef OPENMITTSU_SERVERCONFIGURATION_H_
#define OPENMITTSU_SERVERCONFIGURATION_H_

#include "protocol/ProtocolSpecs.h"
#include "PublicKey.h"
#include <QString>
#include <cstdint>

class ServerConfiguration {
public:
	ServerConfiguration();
	ServerConfiguration(QString const& serverHost, int serverPort, PublicKey const& serverLongTermPublicKey, QString const& identityServerRequestUrl, QString const& identityServerRequestAgent, QString const& identityServerCertificate, QString const& imageServerRequestDownloadUrl, QString const& imageServerRequestDownloadFinishedUrl, QString const& imageServerRequestUploadUrl, QString const& imageServerRequestAgent, QString const& imageServerCertificate);
	static ServerConfiguration fromFile(QString const& filename);
	static void createTemplateConfigurationFile(QString const& filename);

	bool toFile(QString const& filename) const;

	QString const& getServerHost() const;
	int getServerPort() const;
	PublicKey const& getServerLongTermPublicKey() const;

	QString const& getIdentityServerRequestUrl() const;
	QString const& getIdentityServerRequestAgent() const;
	QString const& getIdentityServerCertificateAsBase64() const;

	QString const& getImageServerRequestDownloadUrl() const;
	QString const& getImageServerRequestDownloadFinishedUrl() const;
	QString const& getImageServerRequestUploadUrl() const;
	QString const& getImageServerRequestAgent() const;
	QString const& getImageServerCertificateAsBase64() const;

	QString toString() const;
private:
	QString const serverHost;
	int const serverPort;
	PublicKey const serverLongTermPublicKey;

	QString const identityServerRequestUrl;
	QString const identityServerRequestAgent;
	QString const identityServerCertificate;

	QString const imageServerRequestDownloadUrl;
	QString const imageServerRequestDownloadFinishedUrl;
	QString const imageServerRequestUploadUrl;
	QString const imageServerRequestAgent;
	QString const imageServerCertificate;
};

#endif // OPENMITTSU_SERVERCONFIGURATION_H_