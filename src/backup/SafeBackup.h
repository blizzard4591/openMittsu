#ifndef OPENMITTSU_BACKUP_SAFEBACKUP_H_
#define OPENMITTSU_BACKUP_SAFEBACKUP_H_

#include "src/protocol/ContactId.h"
#include "src/crypto/KeyPair.h"
#include "src/crypto/EncryptionKey.h"
#include <QString>
#include <cstdint>

namespace openmittsu {
	namespace backup {

		class SafeBackup {
		public:
			SafeBackup(openmittsu::protocol::ContactId const& clientContactId, openmittsu::crypto::KeyPair const& clientLongTermKeyPair);
			virtual ~SafeBackup();

			openmittsu::protocol::ContactId const& getClientContactId() const;
			openmittsu::crypto::KeyPair const& getClientLongTermKeyPair() const;

			QByteArray toEncryptedBackup(QString const& safePassword) const;

			static SafeBackup fromEncryptedBackup(QByteArray const& encryptedBackup, openmittsu::crypto::EncryptionKey const& encryptionKey);
		private:
			openmittsu::protocol::ContactId const m_clientContactId;
			openmittsu::crypto::KeyPair const m_clientLongTermKeyPair;
		};

	}
}

#endif // OPENMITTSU_BACKUP_SAFEBACKUP_H_
