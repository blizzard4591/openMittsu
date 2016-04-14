#ifndef OPENMITTSU_CLIENTCONFIGURATION_H_
#define OPENMITTSU_CLIENTCONFIGURATION_H_

#include "protocol/ContactId.h"
#include "protocol/ProtocolSpecs.h"
#include "KeyPair.h"
#include <QString>
#include <cstdint>

class ClientConfiguration {
public:
	class BackupData {
	public:
		BackupData(ContactId const& clientIdentity, KeyPair const& clientLongTermKeyPair) : clientIdentity(clientIdentity), clientLongTermKeyPair(clientLongTermKeyPair) {
		}
		ContactId const clientIdentity;
		KeyPair const clientLongTermKeyPair;
	};

	ClientConfiguration();
	ClientConfiguration(ContactId const& clientIdentity, KeyPair const& clientLongTermKeyPair);
	static ClientConfiguration fromFile(QString const& filename);
	static BackupData fromBackup(QString const& backup, QString const& password);
	static void createTemplateConfigurationFile(QString const& filename);

	bool toFile(QString const& filename) const;
	static bool toFile(QString const& filename, QString const& backupString, QString const& backupPassword);

	ContactId const& getClientIdentity() const;
	KeyPair const& getClientLongTermKeyPair() const;

	QString toString() const;
	QString toBackup(QString const& password) const;
private:
	ContactId const clientIdentity;
	KeyPair const clientLongTermKeyPair;

	static bool isValidIdentity(QString const& identityString);
	static QString generateRandomPassword(int length = 20);
};

#endif // OPENMITTSU_CLIENTCONFIGURATION_H_
