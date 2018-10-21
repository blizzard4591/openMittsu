#include "src/messages/group/groupphoto/GroupEncryptedGroupPhotoAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/groupphoto/GroupGroupPhotoIdAndKeyMessageContent.h"
#include "src/messages/group/groupphoto/GroupSetPhotoMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedGroupPhotoAndKeyMessageContent::GroupEncryptedGroupPhotoAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint32 imageSizeInBytes) : GroupMessageContent(groupId), encryptedImageData(encryptedImage), encryptionKey(encryptionKey), sizeInBytes(imageSizeInBytes) {
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

			openmittsu::tasks::CallbackTask* GroupEncryptedGroupPhotoAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
			}

			bool GroupEncryptedGroupPhotoAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedGroupPhotoAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedImageNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), encryptedImageData, encryptionKey, fixedImageNonce);
			}

			MessageContent* GroupEncryptedGroupPhotoAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupSetPhotoMessageContent.");
					return new GroupSetPhotoMessageContent(getGroupId(), kfndct->getDecryptedData());
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
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

			openmittsu::crypto::EncryptionKey const& GroupEncryptedGroupPhotoAndKeyMessageContent::getEncryptionKey() const {
				return encryptionKey;
			}

			quint32 GroupEncryptedGroupPhotoAndKeyMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			MessageContent* GroupEncryptedGroupPhotoAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedGroupPhotoAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedGroupPhotoAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedGroupPhotoAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
