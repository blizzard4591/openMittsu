#include "src/messages/group/video/GroupEncryptedVideoAndImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/video/GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/group/video/GroupVideoMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedVideoAndImageAndKeyMessageContent::GroupEncryptedVideoAndImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedVideo, QByteArray const& imageData, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes) : GroupMessageContent(groupId), m_encryptedVideoData(encryptedVideo), m_imageData(imageData), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedVideoAndImageAndKeyMessageContent::~GroupEncryptedVideoAndImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedVideoAndImageAndKeyMessageContent::clone() const {
				return new GroupEncryptedVideoAndImageAndKeyMessageContent(getGroupId(), m_encryptedVideoData, m_imageData, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes);
			}

			bool GroupEncryptedVideoAndImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_imageData, fixedNonce);
			}

			bool GroupEncryptedVideoAndImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedVideoData, m_encryptionKey, fixedNonce);
			}

			MessageContent* GroupEncryptedVideoAndImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent.");
					return new GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedVideoData, kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_lengthInSeconds, m_videoSizeInBytes, m_imageData.size());
				} else if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupVideoMessageContent.");
					return new GroupVideoMessageContent(getGroupId(), kfndct->getDecryptedData(), m_imageData, m_lengthInSeconds);
				} else {
					LOGGER()->critical("GroupEncryptedVideoAndImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedVideoAndImageAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& GroupEncryptedVideoAndImageAndKeyMessageContent::getImageData() const {
				return m_imageData;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedVideoAndImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupEncryptedVideoAndImageAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupEncryptedVideoAndImageAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			MessageContent* GroupEncryptedVideoAndImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedVideoAndImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
