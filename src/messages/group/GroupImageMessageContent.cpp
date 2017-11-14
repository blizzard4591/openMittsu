#include "src/messages/group/GroupImageMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/GroupEncryptedImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupImageMessageContent::GroupImageMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& imageData) : GroupMessageContent(groupId), imageData(imageData) {
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

			openmittsu::tasks::CallbackTask* GroupImageMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				QByteArray fixedNonceImage(openmittsu::crypto::Nonce::getNonceLength(), 0x00);
				fixedNonceImage[fixedNonceImage.size() - 1] = 0x01;
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, imageData, fixedNonceImage);
			}

			MessageContent* GroupImageMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
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
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupImageMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupImageMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupImageMessageContent does not support building a packet payload.";
			}

		}
	}
}
