#include "messages/group/GroupEncryptedGroupPhotoAndKeyMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/group/GroupGroupPhotoIdAndKeyMessageContent.h"
#include "messages/group/GroupSetPhotoMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "protocol/FixedNonces.h"
#include "tasks/BlobUploaderCallbackTask.h"
#include "tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "utility/Logging.h"

GroupEncryptedGroupPhotoAndKeyMessageContent::GroupEncryptedGroupPhotoAndKeyMessageContent(GroupId const& groupId, QByteArray const& encryptedImage, EncryptionKey const& encryptionKey, quint32 imageSizeInBytes) : GroupMessageContent(groupId), encryptedImageData(encryptedImage), encryptionKey(encryptionKey), sizeInBytes(imageSizeInBytes) {
	// Intentionally left empty.
}

GroupEncryptedGroupPhotoAndKeyMessageContent::~GroupEncryptedGroupPhotoAndKeyMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupEncryptedGroupPhotoAndKeyMessageContent::clone() const {
	return new GroupEncryptedGroupPhotoAndKeyMessageContent(getGroupId(), encryptedImageData, encryptionKey, sizeInBytes);
}

bool GroupEncryptedGroupPhotoAndKeyMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* GroupEncryptedGroupPhotoAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
}

bool GroupEncryptedGroupPhotoAndKeyMessageContent::hasPostReceiveCallbackTask() const {
	return true;
}

CallbackTask* GroupEncryptedGroupPhotoAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	Nonce const fixedImageNonce = FixedNonces::getFixedGroupImageNonce();
	return new KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<AcknowledgmentProcessor>(), encryptedImageData, encryptionKey, fixedImageNonce);
}

MessageContent* GroupEncryptedGroupPhotoAndKeyMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
		KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupSetPhotoMessageContent.");
		return new GroupSetPhotoMessageContent(getGroupId(), kfndct->getDecryptedData());
	} else if (dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
		BlobUploaderCallbackTask const* buct = dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupGroupPhotoIdAndKeyMessageContent.");
		return new GroupGroupPhotoIdAndKeyMessageContent(getGroupId(), buct->getBlobId(), encryptionKey, sizeInBytes);
	} else {
		LOGGER()->critical("GroupEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& GroupEncryptedGroupPhotoAndKeyMessageContent::getEncryptedImageData() const {
	return encryptedImageData;
}

EncryptionKey const& GroupEncryptedGroupPhotoAndKeyMessageContent::getEncryptionKey() const {
	return encryptionKey;
}

quint32 GroupEncryptedGroupPhotoAndKeyMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

MessageContent* GroupEncryptedGroupPhotoAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content GroupEncryptedGroupPhotoAndKeyMessageContent does not support creation from a packet payload.";
}

QByteArray GroupEncryptedGroupPhotoAndKeyMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content GroupEncryptedGroupPhotoAndKeyMessageContent does not support building a packet payload.";
}
