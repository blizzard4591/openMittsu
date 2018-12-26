#include "src/crypto/SafeMasterKey.h"

#include "sodium.h"
#include "src/exceptions/CryptoException.h"
#include "src/exceptions/InternalErrorException.h"

#include <cstring>

namespace openmittsu {
	namespace crypto {

		SafeMasterKey::SafeMasterKey(QByteArray const& backupId, EncryptionKey const& encryptionKey) : m_backupId(backupId), m_encryptionKey(encryptionKey) {
			//
		}

		SafeMasterKey::~SafeMasterKey() {
			//
		}

		QByteArray const& SafeMasterKey::getBackupId() const {
			return m_backupId;
		}

		EncryptionKey const& SafeMasterKey::getEncryptionKey() const {
			return m_encryptionKey;
		}

		SafeMasterKey SafeMasterKey::fromIdAndPassword(openmittsu::protocol::ContactId const& contact, QString const& safePassword) {
			QByteArray const passwordBytes = safePassword.toUtf8();
			QByteArray const saltBytes = contact.toQString().toUtf8();
			QByteArray resultBytes(SAFE_MASTER_KEY_BYTES, 0x00);
			int result = crypto_pwhash_scryptsalsa208sha256_ll(reinterpret_cast<uint8_t const*>(passwordBytes.data()), passwordBytes.size(), reinterpret_cast<uint8_t const*>(saltBytes.data()), saltBytes.size(), 65536, 8, 1, reinterpret_cast<uint8_t*>(resultBytes.data()), resultBytes.size());
			if (result != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to derive Safe master key, result code was " << result;
			}

			return SafeMasterKey(resultBytes.left(SAFE_BACKUP_ID_BYTES), EncryptionKey(resultBytes.mid(SAFE_BACKUP_ID_BYTES)));
		}

	}
}
