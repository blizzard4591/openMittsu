#include "protocol/AuthenticationPacket.h"

#include "exceptions/InternalErrorException.h"
#include "utility/Version.h"
#include "utility/Logging.h"

AuthenticationPacket::AuthenticationPacket(ContactId const& sender, CryptoBox* cryptoBox) : sender(sender), cryptoBox(cryptoBox) {
	// Intentionally left empty.
}

AuthenticationPacket::~AuthenticationPacket() {
	// Intentionally left empty.
}

QByteArray AuthenticationPacket::toPacket() const {
	QByteArray result;
	result.append(sender.getContactIdAsByteArray());

	if (result.size() != (ContactId::getSizeOfContactIdInBytes())) {
		throw InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #1";
	}

	QString const versionString = QString("%1;O;;%2;").arg(QString::fromStdString(Version::versionWithTagString())).arg(QString::fromStdString(Version::systemName));
	LOGGER_DEBUG("Sending version information: {}", versionString.toStdString());
	result.append(versionString.toUtf8().leftJustified(PROTO_AUTHENTICATION_VERSION_BYTES, 0x00, true));

	if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES))) {
		throw InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #2";
	}

	// A 24-Byte random nonce string
	Nonce const nonce;
	result.append(nonce.getNonce());

	if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES) + Nonce::getNonceLength())) {
		throw InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #3";
	}

	// Server Nonce Prefix is saved in the Connection Context.
	result.append(cryptoBox->getServerNonceGenerator().getNoncePrefix());

	if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES) + NonceGenerator::getNoncePrefixLength() + Nonce::getNonceLength())) {
		throw InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #4";
	}

	// Proof that we have the Client Longterm Secret Key
	QByteArray const proofBox = cryptoBox->encryptForServerWithLongTermKeys(cryptoBox->getClientShortTermKeyPair().getPublicKey(), nonce);
	result.append(proofBox);

	if (result.size() != (PROTO_AUTHENTICATION_UNENCRYPTED_LENGTH_BYTES)) {
		throw InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #5";
	}

	return result;
}
