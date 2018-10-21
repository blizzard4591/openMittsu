#include "src/messages/contact/video/ContactVideoIdAndImageIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndImageIdAndKeyMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactVideoIdAndImageIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_VIDEO, new TypedMessageContentFactory<ContactVideoIdAndImageIdAndKeyMessageContent>());

			ContactVideoIdAndImageIdAndKeyMessageContent::ContactVideoIdAndImageIdAndKeyMessageContent() : ContactMessageContent(), m_videoBlobId(), m_imageBlobId(), m_encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), m_lengthInSeconds(0), m_videoSizeInBytes(0), m_imageSizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactVideoIdAndImageIdAndKeyMessageContent::ContactVideoIdAndImageIdAndKeyMessageContent(QByteArray const& videoBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : ContactMessageContent(), m_videoBlobId(videoBlobId), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				if (m_videoBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_videoBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
				if (m_imageBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_imageBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			ContactVideoIdAndImageIdAndKeyMessageContent::~ContactVideoIdAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactVideoIdAndImageIdAndKeyMessageContent::clone() const {
				return new ContactVideoIdAndImageIdAndKeyMessageContent(m_videoBlobId, m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool ContactVideoIdAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactVideoIdAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_videoBlobId);
			}

			MessageContent* ContactVideoIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_videoSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_videoSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedVideoAndImageIdAndKeyMessageContent.");
					return new ContactEncryptedVideoAndImageIdAndKeyMessageContent(bdct->getDownloadedBlob(), m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("ContactVideoIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactVideoIdAndImageIdAndKeyMessageContent::getVideoBlobId() const {
				return m_videoBlobId;
			}

			QByteArray const& ContactVideoIdAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& ContactVideoIdAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactVideoIdAndImageIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactVideoIdAndImageIdAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 ContactVideoIdAndImageIdAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* ContactVideoIdAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_VIDEO, 1 + 2 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

				int startingPosition = 1;
				uint16_t seconds = openmittsu::utility::ByteArrayConversions::convert2ByteQByteArrayToQuint16(payload.mid(startingPosition, 2));
				startingPosition += 2;
				QByteArray const videoId(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
				uint32_t const videoSize = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
				startingPosition += 4;
				QByteArray const imageId(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
				uint32_t const imageSize = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
				startingPosition += 4;
				openmittsu::crypto::EncryptionKey const key(payload.mid(startingPosition, openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes()));

				return new ContactVideoIdAndImageIdAndKeyMessageContent(videoId, imageId, key, seconds, videoSize, imageSize - crypto_secretbox_MACBYTES);
			}

			QByteArray ContactVideoIdAndImageIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_VIDEO);
				result.append(openmittsu::utility::Endian::uint16FromHostToLittleEndianByteArray(m_lengthInSeconds));
				result.append(m_videoBlobId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(m_videoSizeInBytes));
				result.append(m_imageBlobId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(m_imageSizeInBytes + crypto_secretbox_MACBYTES));
				result.append(m_encryptionKey.getEncryptionKey());

				return result;
			}

		}
	}
}
