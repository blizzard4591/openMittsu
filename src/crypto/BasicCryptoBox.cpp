#include "src/crypto/BasicCryptoBox.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Endian.h"
#include <sodium.h>

namespace openmittsu {
	namespace crypto {

		BasicCryptoBox::BasicCryptoBox(openmittsu::crypto::KeyPair const& clientLongTermKey, openmittsu::crypto::PublicKey const& serverLongTermKey) : m_clientLongTermKey(clientLongTermKey), m_serverLongTermKey(serverLongTermKey) {
			// Intentionally left empty.
		}

		BasicCryptoBox::BasicCryptoBox(BasicCryptoBox const& other) : m_clientLongTermKey(other.m_clientLongTermKey), m_serverLongTermKey(other.m_serverLongTermKey) {
			// Intentionally left empty.
		}

		BasicCryptoBox::~BasicCryptoBox() {
			// Intentionally left empty.
		}

		std::pair<openmittsu::crypto::Nonce, QByteArray> BasicCryptoBox::encrypt(QByteArray const& data, openmittsu::crypto::PublicKey const& targetIdentityPublicKey) {
			openmittsu::crypto::Nonce nonce;
			QByteArray encryptedData(data.size() + crypto_box_MACBYTES, 0x00);
			if (crypto_box_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(targetIdentityPublicKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(m_clientLongTermKey.getPrivateKey().data())) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to encrypt data.";
			}

			return std::make_pair(nonce, encryptedData);
		}

		QByteArray BasicCryptoBox::encryptForFixedNonce(QByteArray const& data, openmittsu::crypto::EncryptionKey const& encryptionKey, openmittsu::crypto::Nonce const& fixedNonce) {
			QByteArray encryptedData(data.size() + crypto_secretbox_MACBYTES, 0x00);
			if (crypto_secretbox_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), fixedNonce.getNonceAsCharPtr(), encryptionKey.getEncryptionKeyAsCharPtr()) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to encrypt data for key and fixed nonce.";
			}

			return encryptedData;
		}

		QByteArray BasicCryptoBox::encrypt(QByteArray const& data, PublicKey const& pubKey, openmittsu::crypto::KeyPair const& privateKey, openmittsu::crypto::Nonce const& nonce) const {
			QByteArray encryptedData(data.size() + crypto_box_MACBYTES, 0x00);
			if (crypto_box_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(pubKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(privateKey.getPrivateKey().data())) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to encrypt data for server.";
			}

			return encryptedData;
		}

		QByteArray BasicCryptoBox::encryptForServerWithLongTermKeys(QByteArray const& data, openmittsu::crypto::Nonce const& nonce) {
			return encrypt(data, m_serverLongTermKey, m_clientLongTermKey, nonce);
		}

		QByteArray BasicCryptoBox::decryptFromFixedNonce(QByteArray const& encryptedData, openmittsu::crypto::EncryptionKey const& encryptionKey, openmittsu::crypto::Nonce const& fixedNonce) {
			if ((encryptedData.size() - crypto_secretbox_MACBYTES) < 1) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data: Cipher text too short.";
			}

			QByteArray decryptedData(encryptedData.size() - crypto_secretbox_MACBYTES, 0x00);
			if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char*>(decryptedData.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), fixedNonce.getNonceAsCharPtr(), encryptionKey.getEncryptionKeyAsCharPtr()) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data for key and fixed nonce.";
			}

			return decryptedData;
		}

	}
}
