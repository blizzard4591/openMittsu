#include "src/messages/contact/ContactAudioIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/contact/ContactEncryptedAudioAndKeyAndAudioIdMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactAudioIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_AUDIO, new TypedMessageContentFactory<ContactAudioIdAndKeyMessageContent>());

			ContactAudioIdAndKeyMessageContent::ContactAudioIdAndKeyMessageContent() : m_blobId(), m_encryptionKey(), m_lengthInSeconds(0), m_sizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactAudioIdAndKeyMessageContent::ContactAudioIdAndKeyMessageContent(QByteArray const& blobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes) : m_blobId(blobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_sizeInBytes(sizeInBytes) {
				if (blobId.size() != PROTO_IMAGESERVER_ID_LENGTH_BYTES) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied image ID has " << blobId.size() << " Bytes instead of " << PROTO_IMAGESERVER_ID_LENGTH_BYTES << " Bytes.";
				}
			}

			ContactAudioIdAndKeyMessageContent::~ContactAudioIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactAudioIdAndKeyMessageContent::clone() const {
				return new ContactAudioIdAndKeyMessageContent(m_blobId, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
			}

			bool ContactAudioIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactAudioIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_blobId);
			}

			MessageContent* ContactAudioIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_sizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_sizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedAudioAndKeyAndAudioIdMessageContent.");
					return new ContactEncryptedAudioAndKeyAndAudioIdMessageContent(bdct->getDownloadedBlob(), m_encryptionKey, m_lengthInSeconds, m_sizeInBytes, m_blobId);
				} else {
					LOGGER()->critical("ContactAudioIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactAudioIdAndKeyMessageContent::getBlobId() const {
				return m_blobId;
			}

			openmittsu::crypto::EncryptionKey const& ContactAudioIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint32 ContactAudioIdAndKeyMessageContent::getBlobSizeInBytes() const {
				return m_sizeInBytes;
			}

			quint16 ContactAudioIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			MessageContent* ContactAudioIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_AUDIO, (1 + 2 + 4 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes()), payload, true);

				int startingPosition = 1;
				uint16_t seconds = openmittsu::utility::ByteArrayConversions::convert2ByteQByteArrayToQuint16(payload.mid(startingPosition, 2));
				startingPosition += 2;
				QByteArray const id(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
				uint32_t const size = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
				startingPosition += 4;
				openmittsu::crypto::EncryptionKey const encryptionKey(payload.mid(startingPosition, openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes()));
				startingPosition += openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes();

				return new ContactAudioIdAndKeyMessageContent(id, encryptionKey, seconds, size);
			}

			QByteArray ContactAudioIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_AUDIO);
				result.append(openmittsu::utility::Endian::uint16FromHostToLittleEndianByteArray(m_lengthInSeconds));
				result.append(m_blobId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(m_sizeInBytes));
				result.append(m_encryptionKey.getEncryptionKey());

				return result;
			}

		}
	}
}
