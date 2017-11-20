#include "src/backup/IdentityBackup.h"

#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/protocol/ProtocolSpecs.h"

#include "src/encoding/Base32.h"
#include "sodium.h"
#include "src/crypto/pbkdf2-sha256.h"

namespace openmittsu {
	namespace backup {

		IdentityBackup::IdentityBackup(openmittsu::protocol::ContactId const& clientContactId, openmittsu::crypto::KeyPair const& clientLongTermKeyPair) : m_clientContactId(clientContactId), m_clientLongTermKeyPair(clientLongTermKeyPair) {
			//
		}

		IdentityBackup::~IdentityBackup() {
			//
		}

		openmittsu::protocol::ContactId const& IdentityBackup::getClientContactId() const {
			return m_clientContactId;
		}

		openmittsu::crypto::KeyPair const& IdentityBackup::getClientLongTermKeyPair() const {
			return m_clientLongTermKeyPair;
		}

		QString IdentityBackup::toBackupString(QString const& password) const {
			QByteArray encryptionKey(BACKUP_ENCRYPTION_KEY_BYTES, 0x00);

			// Generate a Salt
			QByteArray salt(BACKUP_SALT_BYTES, 0x00);
			randombytes_buf(salt.data(), BACKUP_SALT_BYTES);

			// Convert the password into bytes
			QByteArray password8Bit = password.toUtf8();

			// Generate the encryption key for the Backup from the Salt and the Password
			openmittsu_PKCS5_PBKDF2_HMAC(reinterpret_cast<unsigned char*>(password8Bit.data()), password8Bit.size(), reinterpret_cast<unsigned char*>(salt.data()), BACKUP_SALT_BYTES, BACKUP_KEY_PBKDF_ITERATIONS, BACKUP_ENCRYPTION_KEY_BYTES, reinterpret_cast<unsigned char*>(encryptionKey.data()));

			QByteArray nonceBytes(crypto_stream_NONCEBYTES, 0x00);

			// The backup content
			QByteArray clientId(getClientContactId().getContactIdAsByteArray());
			if (clientId.size() != BACKUP_IDENTITY_BYTES) {
				throw openmittsu::exceptions::InternalErrorException() << QString("Could not build backup - invalid client identity length (%1 vs. %2 Bytes).").arg(clientId.size()).arg(BACKUP_IDENTITY_BYTES).toStdString();
			}
			QByteArray clientSecKey(getClientLongTermKeyPair().getPrivateKey());
			if (clientSecKey.size() != PROTO_KEY_LENGTH_BYTES) {
				throw openmittsu::exceptions::InternalErrorException() << QString("Could not build backup - invalid client secret key length (%1 vs. %2 Bytes).").arg(clientSecKey.size()).arg(PROTO_KEY_LENGTH_BYTES).toStdString();
			}

			QByteArray backup(salt);
			backup.append(clientId);
			backup.append(clientSecKey);

			// Compute Hash
			QByteArray controlHash(crypto_hash_sha256_BYTES, 0x00);
			crypto_hash_sha256(reinterpret_cast<unsigned char*>(controlHash.data()), reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES);
			backup.append(controlHash.left(BACKUP_HASH_BYTES));

			if (backup.size() != (BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES)) {
				throw openmittsu::exceptions::InternalErrorException() << QString("Could not build backup - invalid packet length (%1 vs. %2 Bytes).").arg(clientSecKey.size()).arg(BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES).toStdString();
			}

			// The Backup is build from SALT + IDENTITY + KEY + HASH
			crypto_stream_xor(reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES, reinterpret_cast<unsigned char*>(nonceBytes.data()), reinterpret_cast<unsigned char*>(encryptionKey.data()));

			// Encode in Base32
			return openmittsu::encoding::Base32::encodeBase32Sequence(backup);
		}

		bool IdentityBackup::isBackupStringMalformed(QString const& base32Backup) {
			try {
				QByteArray decodedBase32 = openmittsu::encoding::Base32::decodeBase32Sequence(base32Backup);
				if (decodedBase32.size() != BACKUP_DECODED_BYTES) {
					return true;
				}
			} catch (openmittsu::exceptions::InvalidInputException&) {
				return true;
			}

			return false;
		}

		IdentityBackup IdentityBackup::fromBackupString(QString const& backup, QString const& password) {
			QByteArray decodedBase32 = openmittsu::encoding::Base32::decodeBase32Sequence(backup);
			if (decodedBase32.size() != BACKUP_DECODED_BYTES) {
				throw openmittsu::exceptions::InvalidInputException() << "Invalid Backup: Size of decoded Backup String is incorrect (" << decodedBase32.size() << " Bytes instead of " << BACKUP_DECODED_BYTES << " Bytes).";
			}

			unsigned char encryptionKey[BACKUP_ENCRYPTION_KEY_BYTES];
			sodium_memzero(encryptionKey, BACKUP_ENCRYPTION_KEY_BYTES);

			// The pointer to the base32-decoded Backup
			unsigned char* decodedBase32Ptr = reinterpret_cast<unsigned char*>(decodedBase32.data());

			// The Salt used in the PBKDF2 Key Derivation process is embedded in the first 8 bytes of the Backup.
			QByteArray password8Bit = password.toUtf8();
			openmittsu_PKCS5_PBKDF2_HMAC(reinterpret_cast<unsigned char*>(password8Bit.data()), password8Bit.size(), decodedBase32Ptr, BACKUP_SALT_BYTES, BACKUP_KEY_PBKDF_ITERATIONS, BACKUP_ENCRYPTION_KEY_BYTES, encryptionKey);

			unsigned char nonceBytes[crypto_stream_NONCEBYTES];
			sodium_memzero(nonceBytes, crypto_stream_NONCEBYTES);

			crypto_stream_xor(&decodedBase32Ptr[BACKUP_SALT_BYTES], &decodedBase32Ptr[BACKUP_SALT_BYTES], BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES, nonceBytes, encryptionKey);

			// The last two bytes of the Backup contain the truncated SHA-256 Hash over the identity and its Private Key.
			unsigned char controlHash[crypto_hash_sha256_BYTES];
			sodium_memzero(controlHash, crypto_hash_sha256_BYTES);
			crypto_hash_sha256(controlHash, &(decodedBase32Ptr[BACKUP_SALT_BYTES]), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES);

			if (sodium_memcmp(&(decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES]), controlHash, BACKUP_HASH_BYTES) != 0) {
				throw openmittsu::exceptions::InvalidInputException() << "Decryption of Backup failed: The contained control hash is invalid: " << controlHash[0] << controlHash[1] << " vs. " << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES] << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + 1] << "!";
			}

			unsigned char derivedPublicKey[PROTO_KEY_LENGTH_BYTES];
			crypto_scalarmult_base(derivedPublicKey, &decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]);
			openmittsu::crypto::KeyPair kp = openmittsu::crypto::KeyPair::fromArrays(derivedPublicKey, &decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]);

			QString identityString(QByteArray(reinterpret_cast<char*>(&decodedBase32Ptr[BACKUP_SALT_BYTES]), BACKUP_IDENTITY_BYTES));
			if (!openmittsu::protocol::ContactId::isValidContactId(identityString)) {
				throw openmittsu::exceptions::InvalidInputException() << "Decryption of Backup failed: It does not contain a valid Identity.";
			}

			return IdentityBackup(openmittsu::protocol::ContactId(identityString.toUtf8()), kp);
		}

		QString IdentityBackup::generateRandomPassword(std::size_t length) {
			if (length <= 0U) {
				return "";
			}

			QString result;
			result.reserve(static_cast<int>(length));

			for (std::size_t i = 0; i < length; ++i) {
				uint32_t value = 0;
				randombytes_buf(&value, sizeof(uint32_t));
				value = value % (26 + 26 + 10); // a-z, A-Z, 0-9
				if (value < 26) {
					result.append('A' + value);
				} else if (value < 52) {
					result.append('a' + (value - 26));
				} else {
					result.append('0' + (value - 52));
				}
			}

			return result;
		}

	}
}
