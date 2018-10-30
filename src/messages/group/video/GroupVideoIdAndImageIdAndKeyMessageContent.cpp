#include "src/messages/group/video/GroupVideoIdAndImageIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/group/video/GroupEncryptedVideoAndImageIdAndKeyMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

#include <memory>

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupVideoIdAndImageIdAndKeyMessageContent::m_registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_VIDEO, std::make_shared<TypedMessageContentFactory<GroupVideoIdAndImageIdAndKeyMessageContent>>());

			GroupVideoIdAndImageIdAndKeyMessageContent::GroupVideoIdAndImageIdAndKeyMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)), m_videoBlobId(), m_imageBlobId(), m_encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), m_lengthInSeconds(0), m_videoSizeInBytes(0), m_imageSizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupVideoIdAndImageIdAndKeyMessageContent::GroupVideoIdAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& videoBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : GroupMessageContent(groupId), m_videoBlobId(videoBlobId), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				if (m_videoBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_videoBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
				if (m_imageBlobId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied blob ID has " << m_imageBlobId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			GroupVideoIdAndImageIdAndKeyMessageContent::~GroupVideoIdAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupVideoIdAndImageIdAndKeyMessageContent::clone() const {
				return new GroupVideoIdAndImageIdAndKeyMessageContent(getGroupId(), m_videoBlobId, m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool GroupVideoIdAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupVideoIdAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_videoBlobId);
			}

			MessageContent* GroupVideoIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_videoSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_videoSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedVideoAndImageIdAndKeyMessageContent.");
					return new GroupEncryptedVideoAndImageIdAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("GroupVideoIdAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupVideoIdAndImageIdAndKeyMessageContent::getVideoBlobId() const {
				return m_videoBlobId;
			}

			QByteArray const& GroupVideoIdAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& GroupVideoIdAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupVideoIdAndImageIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupVideoIdAndImageIdAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 GroupVideoIdAndImageIdAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* GroupVideoIdAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_VIDEO, 1 + openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes() + 2 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

				int startingPosition = 1;
				openmittsu::protocol::GroupId const group(openmittsu::protocol::GroupId::fromData(payload.mid(startingPosition, openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes())));
				startingPosition += openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes();
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

				return new GroupVideoIdAndImageIdAndKeyMessageContent(group, videoId, imageId, key, seconds, videoSize, imageSize - crypto_secretbox_MACBYTES);
			}

			QByteArray GroupVideoIdAndImageIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_VIDEO);
				result.append(getGroupId().getGroupIdAsByteArray());
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
