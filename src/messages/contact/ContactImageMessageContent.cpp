#include "messages/contact/ContactImageMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/contact/ContactEncryptedImageAndKeyMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "tasks/SymmetricEncryptionCallbackTask.h"
#include "utility/Logging.h"

ContactImageMessageContent::ContactImageMessageContent(QByteArray const& imageData) : imageData(imageData) {
	// Intentionally left empty.
}

ContactImageMessageContent::~ContactImageMessageContent() {
	// Intentionally left empty.
}

ContactMessageContent* ContactImageMessageContent::clone() const {
	return new ContactImageMessageContent(imageData);
}

bool ContactImageMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* ContactImageMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	return new SymmetricEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, imageData, message->getMessageHeader().getReceiver());
}

MessageContent* ContactImageMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<SymmetricEncryptionCallbackTask const*>(callbackTask) != nullptr) {
		SymmetricEncryptionCallbackTask const* sect = dynamic_cast<SymmetricEncryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from SymmetricEncryptionCallbackTask into a new ContactEncryptedImageAndKeyMessageContent.");
		return new ContactEncryptedImageAndKeyMessageContent(sect->getEncryptedData(), sect->getNonce(), static_cast<quint32>(imageData.size()));
	} else {
		LOGGER()->critical("ContactImageMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& ContactImageMessageContent::getImageData() const {
	return imageData;
}

MessageContent* ContactImageMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content ContactImageMessageContent does not support creation from a packet payload.";
}

QByteArray ContactImageMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content ContactImageMessageContent does not support building a packet payload.";
}
