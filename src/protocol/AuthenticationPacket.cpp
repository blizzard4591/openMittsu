#include "src/protocol/AuthenticationPacket.h"

#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Version.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace protocol {

		AuthenticationPacket::AuthenticationPacket(ContactId const& sender, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) : m_sender(sender), m_cryptoBox(cryptoBox) {
			// Intentionally left empty.
		}

		AuthenticationPacket::~AuthenticationPacket() {
			// Intentionally left empty.
		}

		QByteArray AuthenticationPacket::toPacket() const {
			QByteArray result;
			result.append(m_sender.getContactIdAsByteArray());

			if (result.size() != (ContactId::getSizeOfContactIdInBytes())) {
				throw openmittsu::exceptions::InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #1";
			}

			QString const versionString = QString("%1;O;;%2;").arg(QString::fromStdString(openmittsu::utility::Version::versionWithTagString())).arg(QString::fromStdString(openmittsu::utility::Version::systemName));
			LOGGER_DEBUG("Sending version information: {}", versionString.toStdString());
			result.append(versionString.toUtf8().leftJustified(PROTO_AUTHENTICATION_VERSION_BYTES, 0x00, true));

			if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES))) {
				throw openmittsu::exceptions::InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #2";
			}

			// A 24-Byte random nonce string
			openmittsu::crypto::Nonce const nonce;
			result.append(nonce.getNonce());

			if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES) + openmittsu::crypto::Nonce::getNonceLength())) {
				throw openmittsu::exceptions::InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #3";
			}

			// Server Nonce Prefix is saved in the Connection Context.
			result.append(m_cryptoBox->getServerNonceGenerator().getNoncePrefix());

			if (result.size() != (ContactId::getSizeOfContactIdInBytes() + (PROTO_AUTHENTICATION_VERSION_BYTES) + openmittsu::crypto::NonceGenerator::getNoncePrefixLength() + openmittsu::crypto::Nonce::getNonceLength())) {
				throw openmittsu::exceptions::InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #4";
			}

			// Proof that we have the Client Longterm Secret Key
			QByteArray const proofBox = m_cryptoBox->encryptForServerWithLongTermKeys(m_cryptoBox->getClientShortTermKeyPair().getPublicKey(), nonce);
			result.append(proofBox);

			if (result.size() != (PROTO_AUTHENTICATION_UNENCRYPTED_LENGTH_BYTES)) {
				throw openmittsu::exceptions::InternalErrorException() << "Size constraint failed: Authentication Package Size invalid for Field #5";
			}

			return result;
		}

	}
}
