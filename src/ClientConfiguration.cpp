#include "ClientConfiguration.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"

#include "utility/Logging.h"

#include <string>
#include <cstring>
#include <algorithm>

#include "Base32.h"
#include "IdentityHelper.h"
#include "sodium.h"
#include "pbkdf2-sha256.h"

#include <QString>
#include <QStringBuilder>
#include <QFile>
#include <QRegExp>
#include <QSettings>

ClientConfiguration::ClientConfiguration() : clientIdentity(0), clientLongTermKeyPair() {
	//
}

ClientConfiguration::ClientConfiguration(ContactId const& clientIdentity, KeyPair const& clientLongTermKeyPair) : clientIdentity(clientIdentity), clientLongTermKeyPair(clientLongTermKeyPair) {
	//
}

ContactId const& ClientConfiguration::getClientIdentity() const {
	return clientIdentity;
}

KeyPair const& ClientConfiguration::getClientLongTermKeyPair() const {
	return clientLongTermKeyPair;
}

ClientConfiguration ClientConfiguration::fromFile(QString const& filename) {
	if (!QFile::exists(filename)) {
		throw IllegalArgumentException() << "Could not open ClientConfiguration file, file does not exist: " << filename.toStdString();
	}
	QFile inputFile(filename);
	if (!inputFile.open(QFile::ReadOnly)) {
		throw IllegalArgumentException() << "Could not open ClientConfiguration file for reading: " << filename.toStdString();
	}
	inputFile.close();

	QSettings clientSettings(filename, QSettings::IniFormat);

	if (!clientSettings.contains("clientIdentityBackupPassword")) {
		throw IllegalArgumentException() << "Invalid ClientConfiguration: Missing clientIdentityBackupPassword in " << filename.toStdString();
	}

	if (!clientSettings.contains("clientIdentityBackupString")) {
		throw IllegalArgumentException() << "Invalid ClientConfiguration: Missing clientIdentityBackupString in " << filename.toStdString();
	}

	QString const backupString(clientSettings.value("clientIdentityBackupString").toString());
	QString const backupPassword(clientSettings.value("clientIdentityBackupPassword").toString());
	BackupData backupData(ClientConfiguration::fromBackup(backupString, backupPassword));

	return ClientConfiguration(backupData.clientIdentity, backupData.clientLongTermKeyPair);
}

ClientConfiguration::BackupData ClientConfiguration::fromBackup(QString const& backup, QString const& password) {
	QByteArray decodedBase32 = Base32::decodeBase32Sequence(backup);
	if (decodedBase32.size() != BACKUP_DECODED_BYTES) {
		throw IllegalArgumentException() << "Invalid Backup: Size of decoded Backup String is incorrect (" << decodedBase32.size() << " Bytes instead of " << BACKUP_DECODED_BYTES << " Bytes).";
	}

	unsigned char encryptionKey[BACKUP_ENCRYPTION_KEY_BYTES];
	sodium_memzero(encryptionKey, BACKUP_ENCRYPTION_KEY_BYTES);

	// The pointer to the base32-decoded Backup
	unsigned char* decodedBase32Ptr = reinterpret_cast<unsigned char*>(decodedBase32.data());

	// The Salt used in the PBKDF2 Key Derivation process is embedded in the first 8 bytes of the Backup.
	QByteArray password8Bit = password.toUtf8();
	PKCS5_PBKDF2_HMAC(reinterpret_cast<unsigned char*>(password8Bit.data()), password8Bit.size(), decodedBase32Ptr, BACKUP_SALT_BYTES, BACKUP_KEY_PBKDF_ITERATIONS, BACKUP_ENCRYPTION_KEY_BYTES, encryptionKey);

	unsigned char nonceBytes[crypto_stream_NONCEBYTES];
	sodium_memzero(nonceBytes, crypto_stream_NONCEBYTES);

	crypto_stream_xor(&decodedBase32Ptr[BACKUP_SALT_BYTES], &decodedBase32Ptr[BACKUP_SALT_BYTES], BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES, nonceBytes, encryptionKey);

	// The last two bytes of the Backup contain the truncated SHA-256 Hash over the identity and its Private Key.
	unsigned char controlHash[crypto_hash_sha256_BYTES];
	sodium_memzero(controlHash, crypto_hash_sha256_BYTES);
	crypto_hash_sha256(controlHash, &(decodedBase32Ptr[BACKUP_SALT_BYTES]), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES);

	if (sodium_memcmp(&(decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES]), controlHash, BACKUP_HASH_BYTES) != 0) {
		throw IllegalArgumentException() << "Decryption of Backup failed: Invalid Control Hash (" << controlHash[0] << controlHash[1] << " vs. " << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES] << decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + 1] << ").";
	}

	unsigned char derivedPublicKey[PROTO_KEY_LENGTH_BYTES];
	crypto_scalarmult_base(derivedPublicKey, &decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]);
	KeyPair kp = KeyPair::fromArrays(derivedPublicKey, &decodedBase32Ptr[BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES]);

	QString identityString(QByteArray(reinterpret_cast<char*>(&decodedBase32Ptr[BACKUP_SALT_BYTES]), BACKUP_IDENTITY_BYTES));
	if (!isValidIdentity(identityString)) {
		throw IllegalArgumentException() << "Invalid ClientConfiguration: Decryption of Backup failed: Not a valid Identity.";
	}

	return BackupData(ContactId(identityString.toUtf8()), kp);
}

QString ClientConfiguration::toBackup(QString const& password) const {
	QByteArray encryptionKey(BACKUP_ENCRYPTION_KEY_BYTES, 0x00);

	// Generate a Salt
	QByteArray salt(BACKUP_SALT_BYTES, 0x00);
	randombytes_buf(salt.data(), BACKUP_SALT_BYTES);

	// Convert the password into bytes
	QByteArray password8Bit = password.toUtf8();

	// Generate the encryption key for the Backup from the Salt and the Password
	PKCS5_PBKDF2_HMAC(reinterpret_cast<unsigned char*>(password8Bit.data()), password8Bit.size(), reinterpret_cast<unsigned char*>(salt.data()), BACKUP_SALT_BYTES, BACKUP_KEY_PBKDF_ITERATIONS, BACKUP_ENCRYPTION_KEY_BYTES, reinterpret_cast<unsigned char*>(encryptionKey.data()));

	QByteArray nonceBytes(crypto_stream_NONCEBYTES, 0x00);

	// The backup content
	QByteArray clientId(IdentityHelper::uint64ToIdentityString(getClientIdentity().getContactId()).toLatin1());
	if (clientId.size() != BACKUP_IDENTITY_BYTES) {
		throw InternalErrorException() << QString("Could not build backup - invalid client identity length (%1 vs. %2 Bytes).").arg(clientId.size()).arg(BACKUP_IDENTITY_BYTES).toStdString();
	}
	QByteArray clientSecKey(getClientLongTermKeyPair().getPrivateKey());
	if (clientSecKey.size() != PROTO_KEY_LENGTH_BYTES) {
		throw InternalErrorException() << QString("Could not build backup - invalid client secret key length (%1 vs. %2 Bytes).").arg(clientSecKey.size()).arg(PROTO_KEY_LENGTH_BYTES).toStdString();
	}

	QByteArray backup(salt);
	backup.append(clientId);
	backup.append(clientSecKey);

	// Compute Hash
	QByteArray controlHash(crypto_hash_sha256_BYTES, 0x00);
	crypto_hash_sha256(reinterpret_cast<unsigned char*>(controlHash.data()), reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES);
	backup.append(controlHash.left(BACKUP_HASH_BYTES));

	if (backup.size() != (BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES)) {
		throw InternalErrorException() << QString("Could not build backup - invalid packet length (%1 vs. %2 Bytes).").arg(clientSecKey.size()).arg(BACKUP_SALT_BYTES + BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES).toStdString();
	}

	// The Backup is build from SALT + IDENTITY + KEY + HASH
	crypto_stream_xor(reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), reinterpret_cast<unsigned char*>(backup.data() + BACKUP_SALT_BYTES), BACKUP_IDENTITY_BYTES + PROTO_KEY_LENGTH_BYTES + BACKUP_HASH_BYTES, reinterpret_cast<unsigned char*>(nonceBytes.data()), reinterpret_cast<unsigned char*>(encryptionKey.data()));

	// Encode in Base32
	return Base32::encodeBase32Sequence(backup);
}

QString ClientConfiguration::generateRandomPassword(int length) {
	if (length <= 0) {
		return "";
	}

	QString result;
	result.reserve(length);

	for (int i = 0; i < length; ++i) {
		uint16_t value = 0;
		randombytes_buf(&value, sizeof(uint16_t));
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

bool ClientConfiguration::toFile(QString const& filename) const {
	QString backupString;
	QString backupPassword = generateRandomPassword(20);
	try {
		backupString = toBackup(backupPassword);
	} catch (InternalErrorException& iex) {
		LOGGER()->critical("Failed to create a Backup from Identity: {}", iex.what());
		return false;
	}

	return toFile(filename, backupString, backupPassword);
}

bool ClientConfiguration::toFile(QString const& filename, QString const& backupString, QString const& backupPassword) {
	QFile outputFile(filename);
	if (!outputFile.open(QFile::ReadWrite)) {
		LOGGER()->critical("Could not open ClientConfiguration file for writing: {}", filename.toStdString());
		return false;
	}
	outputFile.close();

	QSettings clientSettings(filename, QSettings::IniFormat);
	clientSettings.setValue("clientIdentityBackupString", backupString);
	clientSettings.setValue("clientIdentityBackupPassword", backupPassword);

	clientSettings.sync();
	return true;
}

void ClientConfiguration::createTemplateConfigurationFile(QString const& filename) {
	ClientConfiguration sc(ContactId(QByteArray::fromHex("AAAAAAAA")), KeyPair());
	sc.toFile(filename);
}

bool ClientConfiguration::isValidIdentity(QString const& identityString) {
	static QRegExp identityRegExp("^[a-zA-Z0-9]{8}$", Qt::CaseInsensitive, QRegExp::RegExp2);
	return identityRegExp.exactMatch(identityString);
}

QString ClientConfiguration::toString() const {
	QString result = "ClientConfiguration(clientIdentity = " % IdentityHelper::uint64ToIdentityString(this->getClientIdentity().getContactId()) % ", clientLongTermKeyPair = " % this->getClientLongTermKeyPair().toString() % ")";
	return result;
}
