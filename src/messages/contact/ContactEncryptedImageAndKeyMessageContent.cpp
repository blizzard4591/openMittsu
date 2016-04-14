#include "messages/contact/ContactEncryptedImageAndKeyMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/contact/ContactImageIdAndKeyMessageContent.h"
#include "messages/contact/ContactImageMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "tasks/BlobUploaderCallbackTask.h"
#include "tasks/SymmetricDecryptionCallbackTask.h"
#include "utility/Logging.h"

ContactEncryptedImageAndKeyMessageContent::ContactEncryptedImageAndKeyMessageContent(QByteArray const& encryptedImageData, Nonce const& imageNonce, quint32 imageSizeInBytes) : encryptedImageData(encryptedImageData), nonce(imageNonce), sizeInBytes(imageSizeInBytes) {
	// Intentionally left empty.
}

ContactEncryptedImageAndKeyMessageContent::~ContactEncryptedImageAndKeyMessageContent() {
	// Intentionally left empty.
}

ContactMessageContent* ContactEncryptedImageAndKeyMessageContent::clone() const {
	return new ContactEncryptedImageAndKeyMessageContent(encryptedImageData, nonce, sizeInBytes);
}

bool ContactEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* ContactEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
}

bool ContactEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
	return true;
}

CallbackTask* ContactEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new SymmetricDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<AcknowledgmentProcessor>(), encryptedImageData, nonce, message->getMessageHeader().getSender());
}

MessageContent* ContactEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<SymmetricDecryptionCallbackTask const*>(callbackTask) != nullptr) {
		SymmetricDecryptionCallbackTask const* sdct = dynamic_cast<SymmetricDecryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from SymmetricDecryptionCallbackTask into a new ContactImageMessageContent.");
		return new ContactImageMessageContent(sdct->getDecryptedData());
	} else if (dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
		BlobUploaderCallbackTask const* buct = dynamic_cast<BlobUploaderCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactImageIdAndKeyMessageContent.");
		return new ContactImageIdAndKeyMessageContent(buct->getBlobId(), nonce, sizeInBytes);
	} else {
		LOGGER()->critical("ContactEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& ContactEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
	return encryptedImageData;
}

Nonce const& ContactEncryptedImageAndKeyMessageContent::getNonce() const {
	return nonce;
}

quint32 ContactEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
	return sizeInBytes;
}

MessageContent* ContactEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
}

QByteArray ContactEncryptedImageAndKeyMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyMessageContent does not support building a packet payload.";
}
