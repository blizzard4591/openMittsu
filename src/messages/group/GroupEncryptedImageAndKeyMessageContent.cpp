#include "messages/group/GroupEncryptedImageAndKeyMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/group/GroupImageIdAndKeyMessageContent.h"
#include "messages/group/GroupImageMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "protocol/FixedNonces.h"
#include "tasks/BlobUploaderCallbackTask.h"
#include "tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "utility/Logging.h"

GroupEncryptedImageAndKeyMessageContent::GroupEncryptedImageAndKeyMessageContent(GroupId const& groupId, QByteArray const& encryptedImage, EncryptionKey const& encryptionKey, quint32 imageSizeInBytes) : GroupMessageContent(groupId), encryptedImageData(encryptedImage), encryptionKey(encryptionKey), sizeInBytes(imageSizeInBytes) {
	// Intentionally left empty.
}

GroupEncryptedImageAndKeyMessageContent::~GroupEncryptedImageAndKeyMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupEncryptedImageAndKeyMessageContent::clone() const {
	return new GroupEncryptedImageAndKeyMessageContent(getGroupId(), encryptedImageData, encryptionKey, sizeInBytes);
}

bool GroupEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* GroupEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
}

bool GroupEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
	return true;
}

CallbackTask* GroupEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	Nonce const fixedImageNonce = FixedNonces::getFixedGroupImageNonce();
	return new KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<AcknowledgmentProcessor>(), encryptedImageData, encryptionKey, fixedImageNonce);
}

MessageContent* GroupEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
		KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupImageMessageContent.");
		return new GroupImageMessageContent(getGroupId(), kfndct->getDecryptedData());
	} else if (dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
		BlobUploaderCallbackTask const* buct = dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupImageIdAndKeyMessageContent.");
		return new GroupImageIdAndKeyMessageContent(getGroupId(), buct->getBlobId(), encryptionKey, sizeInBytes);
	} else {
		LOGGER()->critical("GroupEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& GroupEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
	return encryptedImageData;
}

EncryptionKey const& GroupEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
	return encryptionKey;
}

quint32 GroupEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

MessageContent* GroupEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content GroupEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
}

QByteArray GroupEncryptedImageAndKeyMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content GroupEncryptedImageAndKeyMessageContent does not support building a packet payload.";
}
