#include "messages/group/GroupImageMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/group/GroupEncryptedImageAndKeyMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/Nonce.h"
#include "tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "utility/Logging.h"

GroupImageMessageContent::GroupImageMessageContent(GroupId const& groupId, QByteArray const& imageData) : GroupMessageContent(groupId), imageData(imageData) {
	// Intentionally left empty.
}

GroupImageMessageContent::~GroupImageMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupImageMessageContent::clone() const {
	return new GroupImageMessageContent(getGroupId(), imageData);
}

bool GroupImageMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* GroupImageMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	QByteArray fixedNonceImage(Nonce::getNonceLength(), 0x00);
	fixedNonceImage[fixedNonceImage.size() - 1] = 0x01;
	return new KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, imageData, fixedNonceImage);
}

MessageContent* GroupImageMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
		KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedImageAndKeyMessageContent.");
		return new GroupEncryptedImageAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), kfnect->getEncryptionKey(), static_cast<quint32>(imageData.size()));
	} else {
		LOGGER()->critical("GroupImageMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& GroupImageMessageContent::getImageData() const {
	return imageData;
}

MessageContent* GroupImageMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content GroupImageMessageContent does not support creation from a packet payload.";
}

QByteArray GroupImageMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content GroupImageMessageContent does not support building a packet payload.";
}
