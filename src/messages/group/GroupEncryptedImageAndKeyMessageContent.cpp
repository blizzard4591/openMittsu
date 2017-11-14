#include "src/messages/group/GroupEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/GroupImageIdAndKeyMessageContent.h"
#include "src/messages/group/GroupImageMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedImageAndKeyMessageContent::GroupEncryptedImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint32 imageSizeInBytes) : GroupMessageContent(groupId), encryptedImageData(encryptedImage), encryptionKey(encryptionKey), sizeInBytes(imageSizeInBytes) {
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

			openmittsu::tasks::CallbackTask* GroupEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
			}

			bool GroupEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedImageNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), encryptedImageData, encryptionKey, fixedImageNonce);
			}

			MessageContent* GroupEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupImageMessageContent.");
					return new GroupImageMessageContent(getGroupId(), kfndct->getDecryptedData());
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
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

			openmittsu::crypto::EncryptionKey const& GroupEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
				return encryptionKey;
			}

			quint32 GroupEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			MessageContent* GroupEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
