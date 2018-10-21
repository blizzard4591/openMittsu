#include "src/messages/group/video/GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/video/GroupEncryptedVideoAndImageIdAndKeyMessageContent.h"
#include "src/messages/group/video/GroupEncryptedVideoAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedVideo, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : GroupMessageContent(groupId), m_encryptedVideoData(encryptedVideo), m_encryptedImageData(encryptedImage), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::~GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::clone() const {
				return new GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedVideoData, m_encryptedImageData, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedImageData);
			}

			bool GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedImageData, m_encryptionKey, fixedNonce);
			}

			MessageContent* GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupEncryptedVideoAndImageAndKeyMessageContent.");
					return new GroupEncryptedVideoAndImageAndKeyMessageContent(getGroupId(), m_encryptedVideoData, kfndct->getDecryptedData(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupEncryptedVideoAndImageIdAndKeyMessageContent.");
					return new GroupEncryptedVideoAndImageIdAndKeyMessageContent(getGroupId(), m_encryptedVideoData, buct->getBlobId(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
				return m_encryptedImageData;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
