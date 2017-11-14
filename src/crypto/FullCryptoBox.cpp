#include "src/crypto/FullCryptoBox.h"

#include "src/exceptions/CryptoException.h"
#include "src/utility/Endian.h"
#include <sodium.h>

namespace openmittsu {
	namespace crypto {

		FullCryptoBox::FullCryptoBox(openmittsu::dataproviders::KeyRegistry const& keyRegistry)
			: BasicCryptoBox(keyRegistry.getClientLongTermKeyPair(), keyRegistry.getServerLongTermPublicKey()), m_keyRegistry(keyRegistry), m_clientShortTermKey(openmittsu::crypto::KeyPair::randomKey()), m_serverShortTermKey(), m_clientNonceGenerator(), m_serverNonceGenerator() {
			// Intentionally left empty.
		}

		/*FullCryptoBox::FullCryptoBox(FullCryptoBox&& other)
			: BasicCryptoBox(other), m_keyRegistry(std::movcother.m_keyRegistry), m_clientShortTermKey(other.m_clientShortTermKey), m_serverShortTermKey(other.m_serverShortTermKey), m_clientNonceGenerator(other.m_clientNonceGenerator), m_serverNonceGenerator(other.m_serverNonceGenerator) {
		
		}*/

		FullCryptoBox::~FullCryptoBox() {
			// Intentionally left empty.
		}

		std::pair<Nonce, QByteArray> FullCryptoBox::encrypt(QByteArray const& data, openmittsu::protocol::ContactId const& targetIdentity) {
			if (!m_keyRegistry.hasIdentity(targetIdentity)) {
				throw openmittsu::exceptions::CryptoException() << "Can not encrypt for unknown identity.";
			}

			return BasicCryptoBox::encrypt(data, m_keyRegistry.getPublicKeyForIdentity(targetIdentity));
		}

		QByteArray FullCryptoBox::decrypt(QByteArray const& encryptedData, openmittsu::crypto::Nonce const& nonce, openmittsu::protocol::ContactId const& sourceIdentity) {
			if (!m_keyRegistry.hasIdentity(sourceIdentity)) {
				throw openmittsu::exceptions::CryptoException() << "Can not decrypt from unknown identity.";
			} else if ((encryptedData.size() - crypto_box_MACBYTES) < 1) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data: Cipher text too short.";
			}

			QByteArray decryptedData(encryptedData.size() - crypto_box_MACBYTES, 0x00);

			if (crypto_box_open_easy(reinterpret_cast<unsigned char*>(decryptedData.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(m_keyRegistry.getPublicKeyForIdentity(sourceIdentity).getPublicKey().data()), reinterpret_cast<unsigned char const*>(m_keyRegistry.getClientLongTermKeyPair().getPrivateKey().data())) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data.";
			}

			return decryptedData;
		}

		openmittsu::dataproviders::KeyRegistry& FullCryptoBox::getKeyRegistry() {
			return m_keyRegistry;
		}

		openmittsu::crypto::NonceGenerator const& FullCryptoBox::getClientNonceGenerator() const {
			return m_clientNonceGenerator;
		}

		openmittsu::crypto::NonceGenerator const& FullCryptoBox::getServerNonceGenerator() const {
			return m_serverNonceGenerator;
		}

		void FullCryptoBox::setServerNoncePrefixFromServerHello(QByteArray const& newServerNoncePrefix) {
			m_serverNonceGenerator = NonceGenerator(newServerNoncePrefix);
		}

		openmittsu::crypto::KeyPair const& FullCryptoBox::getClientShortTermKeyPair() const {
			return m_clientShortTermKey;
		}

		openmittsu::crypto::PublicKey const& FullCryptoBox::getServerShortTermPublicKey() const {
			return m_serverShortTermKey;
		}

		void FullCryptoBox::setServerShortTermPublicKey(openmittsu::crypto::PublicKey const& newServerKey) {
			m_serverShortTermKey = newServerKey;
		}

		QByteArray FullCryptoBox::encrypt(QByteArray const& data, PublicKey const& pubKey, openmittsu::crypto::KeyPair const& privateKey, openmittsu::crypto::Nonce const& nonce) const {
			QByteArray encryptedData(data.size() + crypto_box_MACBYTES, 0x00);
			if (crypto_box_easy(reinterpret_cast<unsigned char*>(encryptedData.data()), reinterpret_cast<unsigned char const*>(data.data()), data.size(), nonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(pubKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(privateKey.getPrivateKey().data())) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to encrypt data for server.";
			}

			return encryptedData;
		}

		QByteArray FullCryptoBox::encryptForServer(QByteArray const& data) {
			Nonce clientNonce(m_clientNonceGenerator.getNextNonce());

			return encrypt(data, m_serverShortTermKey, m_clientShortTermKey, clientNonce);
		}

		QByteArray FullCryptoBox::decryptFromServer(QByteArray const& encryptedData, openmittsu::crypto::PublicKey const& serverKey) {
			if ((encryptedData.size() - crypto_box_MACBYTES) < 1) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data: Cipher text too short.";
			}

			openmittsu::crypto::Nonce const serverNonce(m_serverNonceGenerator.getNextNonce());
			QByteArray decodedPacket(encryptedData.size() - crypto_box_MACBYTES, 0x00);
			if (crypto_box_open_easy(reinterpret_cast<unsigned char*>(decodedPacket.data()), reinterpret_cast<unsigned char const*>(encryptedData.data()), encryptedData.size(), serverNonce.getNonceAsCharPtr(), reinterpret_cast<unsigned char const*>(serverKey.getPublicKey().data()), reinterpret_cast<unsigned char const*>(getClientShortTermKeyPair().getPrivateKey().data())) != 0) {
				throw openmittsu::exceptions::CryptoException() << "Failed to decrypt data.";
			}

			return decodedPacket;
		}

		QByteArray FullCryptoBox::decryptFromServer(QByteArray const& encryptedData) {
			return decryptFromServer(encryptedData, m_serverShortTermKey);
		}

	}
}
