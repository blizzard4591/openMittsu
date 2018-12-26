#include "src/backup/SafeBackup.h"

#include "src/crypto/Nonce.h"
#include "src/exceptions/CryptoException.h"
#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/utility/Logging.h"

#include "sodium.h"

namespace openmittsu {
	namespace backup {

		SafeBackup::SafeBackup(openmittsu::protocol::ContactId const& clientContactId, openmittsu::crypto::KeyPair const& clientLongTermKeyPair) : m_clientContactId(clientContactId), m_clientLongTermKeyPair(clientLongTermKeyPair) {
			//
		}

		SafeBackup::~SafeBackup() {
			//
		}

		openmittsu::protocol::ContactId const& SafeBackup::getClientContactId() const {
			return m_clientContactId;
		}

		openmittsu::crypto::KeyPair const& SafeBackup::getClientLongTermKeyPair() const {
			return m_clientLongTermKeyPair;
		}

		QByteArray SafeBackup::toEncryptedBackup(QString const& safePassword) const {
			return QByteArray();
		}

		SafeBackup SafeBackup::fromEncryptedBackup(QByteArray const& encryptedBackup, openmittsu::crypto::EncryptionKey const& encryptionKey) {
			QByteArray const nonceData = encryptedBackup.left(SAFE_NONCE_BYTES);
			openmittsu::crypto::Nonce nonce(nonceData);
			QByteArray const encryptedData = encryptedBackup.mid(SAFE_NONCE_BYTES);

			QByteArray decryptedData(encryptedData.size() - crypto_secretbox_MACBYTES, 0x00);
			if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char*>(decryptedData.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), nonce.getNonceAsCharPtr(), encryptionKey.getEncryptionKeyAsCharPtr()) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt safe backup data for key and given nonce.";
			}

			LOGGER_DEBUG("Decrypted Gzipped Stuff: {}", QString(decryptedData.toHex()).toStdString());

			return SafeBackup(openmittsu::protocol::ContactId(), openmittsu::crypto::KeyPair::randomKey());
		}

	}
}
