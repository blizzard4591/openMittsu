#include "messages/group/GroupSetPhotoMessageContent.h"

#include "exceptions/IllegalFunctionCallException.h"
#include "messages/group/GroupEncryptedGroupPhotoAndKeyMessageContent.h"
#include "protocol/CryptoBox.h"
#include "protocol/FixedNonces.h"
#include "protocol/Nonce.h"
#include "tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "utility/Logging.h"

GroupSetPhotoMessageContent::GroupSetPhotoMessageContent(GroupId const& groupId, QByteArray const& groupPhotoData) : GroupMessageContent(groupId), groupPhoto(groupPhotoData) {
	// Intentionally left empty.
}

GroupSetPhotoMessageContent::~GroupSetPhotoMessageContent() {
	// Intentionally left empty.
}

GroupMessageContent* GroupSetPhotoMessageContent::clone() const {
	return new GroupSetPhotoMessageContent(getGroupId(), groupPhoto);
}

bool GroupSetPhotoMessageContent::hasPreSendCallbackTask() const {
	return true;
}

CallbackTask* GroupSetPhotoMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const {
	Nonce const fixedImageNonce = FixedNonces::getFixedGroupImageNonce();
	return new KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, groupPhoto, fixedImageNonce);
}

MessageContent* GroupSetPhotoMessageContent::integrateCallbackTaskResult(CallbackTask const* callbackTask) const {
	if (dynamic_cast<KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
		KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
		LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedGroupPhotoAndKeyMessageContent.");
		return new GroupEncryptedGroupPhotoAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), kfnect->getEncryptionKey(), static_cast<quint32>(groupPhoto.size()));
	} else {
		LOGGER()->critical("GroupImageMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
		throw;
	}
}

QByteArray const& GroupSetPhotoMessageContent::getGroupPhoto() const {
	return groupPhoto;
}

MessageContent* GroupSetPhotoMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
	throw IllegalFunctionCallException() << "The intermediate content GroupSetPhotoMessageContent does not support creation from a packet payload.";
}

QByteArray GroupSetPhotoMessageContent::toPacketPayload() const {
	throw IllegalFunctionCallException() << "The intermediate content GroupSetPhotoMessageContent does not support building a packet payload.";
}
