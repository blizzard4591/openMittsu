#ifndef OPENMITTSU_CRYPTO_SAFEMASTERKEY_H_
#define OPENMITTSU_CRYPTO_SAFEMASTERKEY_H_

#include "src/protocol/ProtocolSpecs.h"
#include "src/protocol/ContactId.h"
#include "src/crypto/EncryptionKey.h"
#include <QString>
#include <QByteArray>

namespace openmittsu {
	namespace crypto {

		class SafeMasterKey {
		public:
			SafeMasterKey(QByteArray const& backupId, EncryptionKey const& encryptionKey);
			virtual ~SafeMasterKey();

			virtual QByteArray const& getBackupId() const;
			virtual EncryptionKey const& getEncryptionKey() const;

			static SafeMasterKey fromIdAndPassword(openmittsu::protocol::ContactId const& contact, QString const& safePassword);
		protected:
			QByteArray const m_backupId;
			EncryptionKey const m_encryptionKey;
		};

	}
}

#endif // OPENMITTSU_CRYPTO_SAFEMASTERKEY_H_
