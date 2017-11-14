#include "src/messages/group/GroupGroupPhotoIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/group/GroupEncryptedGroupPhotoAndKeyMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			// Register this MessageContent with the MessageContentRegistry
			bool GroupGroupPhotoIdAndKeyMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO, new TypedMessageContentFactory<GroupGroupPhotoIdAndKeyMessageContent>());

			GroupGroupPhotoIdAndKeyMessageContent::GroupGroupPhotoIdAndKeyMessageContent() : GroupMessageContent(openmittsu::protocol::GroupId(0, 0)), imageId(), encryptionKey(QByteArray(openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), sizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			GroupGroupPhotoIdAndKeyMessageContent::GroupGroupPhotoIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& imageId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint32 sizeInBytes) : GroupMessageContent(groupId), imageId(imageId), encryptionKey(encryptionKey), sizeInBytes(sizeInBytes) {
				if (imageId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied image ID has " << imageId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
				}
			}

			GroupGroupPhotoIdAndKeyMessageContent::~GroupGroupPhotoIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupGroupPhotoIdAndKeyMessageContent::clone() const {
				return new GroupGroupPhotoIdAndKeyMessageContent(getGroupId(), imageId, encryptionKey, sizeInBytes);
			}

			bool GroupGroupPhotoIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupGroupPhotoIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), imageId);
			}

			MessageContent* GroupGroupPhotoIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(sizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), sizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedGroupPhotoAndKeyMessageContent.");
					return new GroupEncryptedGroupPhotoAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), encryptionKey, sizeInBytes);
				} else {
					LOGGER()->critical("GroupImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupGroupPhotoIdAndKeyMessageContent::getImageId() const {
				return imageId;
			}

			openmittsu::crypto::EncryptionKey const& GroupGroupPhotoIdAndKeyMessageContent::getEncryptionKey() const {
				return encryptionKey;
			}

			quint32 GroupGroupPhotoIdAndKeyMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			MessageContent* GroupGroupPhotoIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO, 1 + openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes() + (PROTO_IMAGESERVER_ID_LENGTH_BYTES)+4 + openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

				int startingPosition = 1;
				openmittsu::protocol::GroupId const group(openmittsu::protocol::GroupId::fromData(payload.mid(startingPosition, openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes())));
				startingPosition += openmittsu::protocol::GroupId::getSizeOfGroupIdInBytes();
				QByteArray const id(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
				uint32_t const size = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
				startingPosition += 4;
				openmittsu::crypto::EncryptionKey const key(payload.mid(startingPosition, openmittsu::crypto::EncryptionKey::getSizeOfEncryptionKeyInBytes()));

				return new GroupGroupPhotoIdAndKeyMessageContent(group, id, key, size);
			}

			QByteArray GroupGroupPhotoIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO);
				result.append(getGroupId().getGroupIdAsByteArray());
				result.append(imageId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(sizeInBytes));
				result.append(encryptionKey.getEncryptionKey());

				return result;
			}

		}
	}
}
