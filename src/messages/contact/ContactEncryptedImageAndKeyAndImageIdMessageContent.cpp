#include "messages/contact/ContactEncryptedImageAndKeyAndImageIdMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/contact/ContactEncryptedImageAndKeyMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "tasks/BlobDeleterCallbackTask.h"
#include "utility/Logging.h"

ContactEncryptedImageAndKeyAndImageIdMessageContent::ContactEncryptedImageAndKeyAndImageIdMessageContent(QByteArray const& encryptedImageData, Nonce const& imageNonce, quint32 imageSizeInBytes, QByteArray const& blobId) : encryptedImageData(encryptedImageData), nonce(imageNonce), sizeInBytes(imageSizeInBytes), blobId(blobId) {
	// Intentionally left empty.
}

ContactEncryptedImageAndKeyAndImageIdMessageContent::~ContactEncryptedImageAndKeyAndImageIdMessageContent() {
	// Intentionally left empty.
}

ContactMessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::clone() const {
	return new ContactEncryptedImageAndKeyAndImageIdMessageContent(encryptedImageData, nonce, sizeInBytes, blobId);
}

bool ContactEncryptedImageAndKeyAndImageIdMessageContent::hasPostReceiveCallbackTask() const {
	return true;
}

CallbackTask* ContactEncryptedImageAndKeyAndImageIdMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobDeleterCallbackTask(serverConfiguration, message, std::shared_ptr<AcknowledgmentProcessor>(), blobId);
}

MessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<BlobDeleterCallbackTask const*>(callbackTask) != nullptr) {
		LOGGER_DEBUG("BlobDeleterCallbackTask finished, building previously downloaded blob into a new ContactEncryptedImageAndKeyMessageContent.");
		return new ContactEncryptedImageAndKeyMessageContent(encryptedImageData, nonce, sizeInBytes);
	} else {
		LOGGER()->critical("ContactEncryptedImageAndKeyAndImageIdMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getEncryptedImageData() const {
	return encryptedImageData;
}

Nonce const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getNonce() const {
	return nonce;
}

quint32 ContactEncryptedImageAndKeyAndImageIdMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

QByteArray const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getImageId() const {
	return blobId;
}

MessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyAndImageIdMessageContent does not support creation from a packet payload.";
}

QByteArray ContactEncryptedImageAndKeyAndImageIdMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyAndImageIdMessageContent does not support building a packet payload.";
}
