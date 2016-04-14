#include "messages/group/GroupImageIdAndKeyMessageContent.h"

#include "Endian.h"
#include "exceptions/IllegalArgumentException.h"
#include "messages/MessageContentRegistry.h"
#include "messages/group/GroupEncryptedImageAndKeyMessageContent.h"
#include "protocol/ProtocolSpecs.h"
#include "tasks/BlobDownloaderCallbackTask.h"
#include "utility/ByteArrayConversions.h"
#include "utility/Logging.h"

// Register this MessageContent with the MessageContentRegistry
bool GroupImageIdAndKeyMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_GROUP_PICTURE, new TypedMessageContentFactory<GroupImageIdAndKeyMessageContent>());

GroupImageIdAndKeyMessageContent::GroupImageIdAndKeyMessageContent() : GroupMessageContent(GroupId(0, 0)), imageId(), encryptionKey(QByteArray(EncryptionKey::getSizeOfEncryptionKeyInBytes(), 0x00)), sizeInBytes(0) {
	// Only accessible and used by the MessageContentFactory.
}

GroupImageIdAndKeyMessageContent::GroupImageIdAndKeyMessageContent(GroupId const& groupId, QByteArray const& imageId, EncryptionKey const& encryptionKey, quint32 sizeInBytes) : GroupMessageContent(groupId), imageId(imageId), encryptionKey(encryptionKey), sizeInBytes(sizeInBytes) {
	if (imageId.size() != (PROTO_IMAGESERVER_ID_LENGTH_BYTES)) {
		throw IllegalArgumentException() << "The supplied image ID has " << imageId.size() << " Bytes instead of " << (PROTO_IMAGESERVER_ID_LENGTH_BYTES) << " Bytes.";
	}
}

GroupImageIdAndKeyMessageContent::~GroupImageIdAndKeyMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupImageIdAndKeyMessageContent::clone() const {
	return new GroupImageIdAndKeyMessageContent(getGroupId(), imageId, encryptionKey, sizeInBytes);
}

bool GroupImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
	return true;
}

CallbackTask* GroupImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<AcknowledgmentProcessor>(), imageId);
}

MessageContent* GroupImageIdAndKeyMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
		BlobDownloaderCallbackTask const* bdct = dynamic_cast<BlobDownloaderCallbackTask const*>(callbackTask);
		if (bdct->getDownloadedBlob().size() != static_cast<int>(sizeInBytes)) {
			LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), sizeInBytes);
		}

		LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedImageAndKeyMessageContent.");
		return new GroupEncryptedImageAndKeyMessageContent(getGroupId(), bdct->getDownloadedBlob(), encryptionKey, sizeInBytes);
	} else {
		LOGGER()->critical("GroupImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& GroupImageIdAndKeyMessageContent::getImageId() const {
	return imageId;
}

EncryptionKey const& GroupImageIdAndKeyMessageContent::getEncryptionKey() const {
	return encryptionKey;
}

quint32 GroupImageIdAndKeyMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

MessageContent* GroupImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_GROUP_PICTURE, 1 + GroupId::getSizeOfGroupIdInBytes() + (PROTO_IMAGESERVER_ID_LENGTH_BYTES) + 4 + EncryptionKey::getSizeOfEncryptionKeyInBytes(), payload, true);

	int startingPosition = 1;
	GroupId const group(GroupId::fromData(payload.mid(startingPosition, GroupId::getSizeOfGroupIdInBytes())));
	startingPosition += GroupId::getSizeOfGroupIdInBytes();
	QByteArray const id(payload.mid(startingPosition, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
	startingPosition += PROTO_IMAGESERVER_ID_LENGTH_BYTES;
	uint32_t const size = ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(startingPosition, 4));
	startingPosition += 4;
	EncryptionKey const key(payload.mid(startingPosition, EncryptionKey::getSizeOfEncryptionKeyInBytes()));

	return new GroupImageIdAndKeyMessageContent(group, id, key, size);
}

QByteArray GroupImageIdAndKeyMessageContent::toPacketPayload() const {
	QByteArray result(1, PROTO_MESSAGE_SIGNATURE_GROUP_PICTURE);
	result.append(getGroupId().getGroupIdAsByteArray());
	result.append(imageId);
	result.append(Endian::uint32FromHostToLittleEndianByteArray(sizeInBytes));
	result.append(encryptionKey.getEncryptionKey());

	return result;
}
