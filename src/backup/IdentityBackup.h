#ifndef OPENMITTSU_BACKUP_IDENTITYBACKUP_H_
#define OPENMITTSU_BACKUP_IDENTITYBACKUP_H_

#include "src/protocol/ContactId.h"
#include "src/crypto/KeyPair.h"
#include <QString>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		class IdentityBackup {
		public:
			IdentityBackup(openmittsu::protocol::ContactId const& clientContactId, openmittsu::crypto::KeyPair const& clientLongTermKeyPair);
			virtual ~IdentityBackup();

			openmittsu::protocol::ContactId const& getClientContactId() const;
			openmittsu::crypto::KeyPair const& getClientLongTermKeyPair() const;

			QString toBackupString(QString const& password) const;

			static bool isBackupStringMalformed(QString const& base32Backup);
			static IdentityBackup fromBackupString(QString const& backup, QString const& password);

			static QString generateRandomPassword(std::size_t length = 20U);
		private:
			openmittsu::protocol::ContactId const m_clientContactId;
			openmittsu::crypto::KeyPair const m_clientLongTermKeyPair;
		};

	}
}

#endif // OPENMITTSU_BACKUP_IDENTITYBACKUP_H_
