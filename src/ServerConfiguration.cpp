#include "ServerConfiguration.h"

#include "exceptions/IllegalArgumentException.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <QString>
#include <QStringBuilder>
#include <QFile>
#include <QSettings>

ServerConfiguration::ServerConfiguration() : serverHost(""), serverPort(-1), serverLongTermPublicKey(), identityServerRequestUrl(""), identityServerRequestAgent(""), identityServerCertificate(""), imageServerRequestDownloadUrl(""), imageServerRequestDownloadFinishedUrl(""), imageServerRequestUploadUrl(""), imageServerRequestAgent(""), imageServerCertificate("") {
	//
}

ServerConfiguration::ServerConfiguration(QString const& serverHost, int serverPort, PublicKey const& serverLongTermPublicKey, QString const& identityServerRequestUrl, QString const& identityServerRequestAgent, QString const& identityServerCertificate, QString const& imageServerRequestDownloadUrl, QString const& imageServerRequestDownloadFinishedUrl, QString const& imageServerRequestUploadUrl, QString const& imageServerRequestAgent, QString const& imageServerCertificate)
	: serverHost(serverHost), serverPort(serverPort), serverLongTermPublicKey(serverLongTermPublicKey), identityServerRequestUrl(identityServerRequestUrl), identityServerRequestAgent(identityServerRequestAgent), identityServerCertificate(identityServerCertificate), imageServerRequestDownloadUrl(imageServerRequestDownloadUrl), imageServerRequestDownloadFinishedUrl(imageServerRequestDownloadFinishedUrl), imageServerRequestUploadUrl(imageServerRequestUploadUrl), imageServerRequestAgent(imageServerRequestAgent), imageServerCertificate(imageServerCertificate) {
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

QString const& ServerConfiguration::getIdentityServerRequestUrl() const {
	return identityServerRequestUrl;
}

QString const& ServerConfiguration::getIdentityServerRequestAgent() const {
	return identityServerRequestAgent;
}

QString const& ServerConfiguration::getIdentityServerCertificateAsBase64() const {
	return identityServerCertificate;
}

QString const& ServerConfiguration::getImageServerRequestDownloadUrl() const {
	return imageServerRequestDownloadUrl;
}

QString const& ServerConfiguration::getImageServerRequestDownloadFinishedUrl() const {
	return imageServerRequestDownloadFinishedUrl;
}

QString const& ServerConfiguration::getImageServerRequestUploadUrl() const {
	return imageServerRequestUploadUrl;
}

QString const& ServerConfiguration::getImageServerRequestAgent() const {
	return imageServerRequestAgent;
}

QString const& ServerConfiguration::getImageServerCertificateAsBase64() const {
	return imageServerCertificate;
}

ServerConfiguration ServerConfiguration::fromFile(QString const& filename) {
	if (!QFile::exists(filename)) {
		throw IllegalArgumentException() << "Could not open ServerConfiguration file, file does not exist: " << filename.toStdString();
	}
	QFile inputFile(filename);
	if (!inputFile.open(QFile::ReadOnly)) {
		throw IllegalArgumentException() << "Could not open ServerConfiguration file for reading: " << filename.toStdString();
	}
	inputFile.close();

	QSettings serverSettings(filename, QSettings::IniFormat);
	if (!serverSettings.contains("serverHost")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing serverHost in " << filename.toStdString();
	}
	
	if (!serverSettings.contains("serverPort")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing serverPort in " << filename.toStdString();
	}
	bool ok = false;
	int port = serverSettings.value("serverPort").toInt(&ok);
	if (!ok || (port < 1) || (port > 65535)) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Invalid serverPort in " << filename.toStdString();
	}

	if (!serverSettings.contains("serverLongTermPublicKey")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing serverLongTermPublicKey in " << filename.toStdString();
	}

	if (!serverSettings.contains("identityRequestUrl")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing identityRequestUrl in " << filename.toStdString();
	}
	if (!serverSettings.contains("identityRequestAgent")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing identityRequestAgent in " << filename.toStdString();
	}
	if (!serverSettings.contains("identityServerCertificate")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing identityServerCertificate in " << filename.toStdString();
	}

	if (!serverSettings.contains("imageRequestDownloadUrl")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing imageRequestDownloadUrl in " << filename.toStdString();
	}
	if (!serverSettings.contains("imageRequestDownloadFinishedUrl")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing imageRequestDownloadFinishedUrl in " << filename.toStdString();
	}
	if (!serverSettings.contains("imageRequestUploadUrl")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing imageRequestUploadUrl in " << filename.toStdString();
	}
	if (!serverSettings.contains("imageRequestAgent")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing imageRequestAgent in " << filename.toStdString();
	}
	if (!serverSettings.contains("imageServerCertificate")) {
		throw IllegalArgumentException() << "Invalid ServerConfiguration: Missing imageServerCertificate in " << filename.toStdString();
	}

	return ServerConfiguration(serverSettings.value("serverHost").toString(), port, PublicKey::fromHexString(serverSettings.value("serverLongTermPublicKey").toString()), serverSettings.value("identityRequestUrl").toString(), serverSettings.value("identityRequestAgent").toString(), serverSettings.value("identityServerCertificate").toString(), serverSettings.value("imageRequestDownloadUrl").toString(), serverSettings.value("imageRequestDownloadFinishedUrl").toString(), serverSettings.value("imageRequestUploadUrl").toString(), serverSettings.value("imageRequestAgent").toString(), serverSettings.value("imageServerCertificate").toString());
}

bool ServerConfiguration::toFile(QString const& filename) const {
	QFile outputFile(filename);
	if (!outputFile.open(QFile::ReadWrite)) {
		return false;
	}
	outputFile.close();

	QSettings serverSettings(filename, QSettings::IniFormat);
	serverSettings.setValue("serverHost", serverHost);
	serverSettings.setValue("serverPort", serverPort);
	serverSettings.setValue("serverLongTermPublicKey", QString(serverLongTermPublicKey.getPublicKey().toHex()));
	serverSettings.setValue("identityRequestUrl", identityServerRequestUrl);
	serverSettings.setValue("identityRequestAgent", identityServerRequestAgent);
	serverSettings.setValue("identityServerCertificate", identityServerCertificate);
	serverSettings.setValue("imageRequestDownloadUrl", imageServerRequestDownloadUrl);
	serverSettings.setValue("imageRequestDownloadFinishedUrl", imageServerRequestDownloadFinishedUrl);
	serverSettings.setValue("imageRequestUploadUrl", imageServerRequestUploadUrl);
	serverSettings.setValue("imageRequestAgent", imageServerRequestAgent);
	serverSettings.setValue("imageServerCertificate", imageServerCertificate);
	serverSettings.sync();

	return true;
}

void ServerConfiguration::createTemplateConfigurationFile(QString const& filename) {
	ServerConfiguration sc("localhost", 12345, PublicKey(), "https://localhost/identity/%1", "OpenMittsu/1.0A", "", "https://localhost/%1", "https://localhost/%1/done", "https://localhost/upload", "OpenMittsu/1.0A", "");
	sc.toFile(filename);
}

QString ServerConfiguration::toString() const {
	QString result = "ServerConfiguration(serverHost = " % this->getServerHost() %
		", serverPort = " % QString::number(this->getServerPort()) % 
		", serverLongTermPublicKey = " % this->getServerLongTermPublicKey().toString() % 
		", identityRequestUrl = " % this->getIdentityServerRequestUrl() % 
		", identityRequestAgent = " % this->getIdentityServerRequestAgent() % 
		", identityServerCertificate = " % this->getIdentityServerCertificateAsBase64() %
		", imageRequestDownloadUrl = " % this->getImageServerRequestDownloadUrl() % 
		", imageRequestDownloadFinishedUrl = " % this->getImageServerRequestDownloadFinishedUrl() % 
		", imageRequestUploadUrl = " % this->getImageServerRequestUploadUrl() % 
		", imageRequestAgent = " % this->getImageServerRequestAgent() % 
		", imageServerCertificate = " % this->getImageServerCertificateAsBase64() % ")";
	return result;
}