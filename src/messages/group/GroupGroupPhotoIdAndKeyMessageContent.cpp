#include "messages/group/GroupGroupPhotoIdAndKeyMessageContent.h"

#include "Endian.h"
#include "exceptions/IllegalArgumentException.h"
#include "messages/MessageContentRegistry.h"
#include "messages/group/GroupEncryptedGroupPhotoAndKeyMessageContent.h"
#include "protocol/ProtocolSpecs.h"
#include "tasks/BlobDownloaderCallbackTask.h"
#include "utility/ByteArrayConversions.h"
#include "utility/Logging.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupGroupPhotoIdAndKeyMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO, new TypedMessageContentFactory<GroupGroupPhotoIdAndKeyMessageContent>());

GroupGroupPhotoIdAndKeyMessageContent::GroupGroupPhotoIdAndKeyMessageContent() : GroupMessageContent(GroupId(0, 0)), imageId(), encryptionKey(QByteArray(EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), sizeInBytes(0) {
	// Only accessible and used by the MessageContentFactory.
}

GroupGroupPhotoIdAndKeyMessageContent::GroupGroupPhotoIdAndKeyMessageContent(GroupId const& groupId, QByteArray const& imageId, EncryptionKey const& encryptionKey, quint32 sizeInBytes) : GroupMessageContent(groupId), imageId(imageId), encryptionKey(encryptionKey), sizeInBytes(sizeInBytes) {
	if (imageId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
		throw IllegalArgumentException() << "The supplied image ID has " << imageId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
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

CallbackTask* GroupGroupPhotoIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<AcknowledgmentProcessor>(), imageId);
}

MessageContent* GroupGroupPhotoIdAndKeyMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
		BlobDownloaderCallbackTask const* bdct = dynamic_cast<BlobDownloaderCallbackTask const*>(callbackTask);
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

EncryptionKey const& GroupGroupPhotoIdAndKeyMessageContent::getEncryptionKey() const {
	return encryptionKey;
}

quint32 GroupGroupPhotoIdAndKeyMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

MessageContent* GroupGroupPhotoIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO, 1 + GroupId::getSizeOfGroupIdInBytes() + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

	int startingPosition = 1;
	GroupId const group(GroupId::fromData(payload.mid(startingPosition, GroupId::getSizeOfGroupIdInBytes())));
	startingPosition += GroupId::getSizeOfGroupIdInBytes();
	QByteArray const id(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
	startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
	uint32_t const size = ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
	startingPosition += 4;
	EncryptionKey const key(payload.mid(startingPosition, EncryptionKey::getSizeOfEncryptionKeyInBytes()));

	return new GroupGroupPhotoIdAndKeyMessageContent(group, id, key, size);
}

QByteArray GroupGroupPhotoIdAndKeyMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_PHOTO);
	result.append(getGroupId().getGroupIdAsByteArray());
	result.append(imageId);
	result.append(Endian::uint32FromHostToLittleEndianByteArray(sizeInBytes));
	result.append(encryptionKey.getEncryptionKey());

	return result;
}
