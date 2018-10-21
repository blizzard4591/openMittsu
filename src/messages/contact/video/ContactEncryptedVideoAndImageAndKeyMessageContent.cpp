#include "src/messages/contact/video/ContactEncryptedVideoAndImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/contact/video/ContactVideoMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedVideoAndImageAndKeyMessageContent::ContactEncryptedVideoAndImageAndKeyMessageContent(QByteArray const& encryptedVideo, QByteArray const& imageData, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes) : ContactMessageContent(), m_encryptedVideoData(encryptedVideo), m_imageData(imageData), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedVideoAndImageAndKeyMessageContent::~ContactEncryptedVideoAndImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedVideoAndImageAndKeyMessageContent::clone() const {
				return new ContactEncryptedVideoAndImageAndKeyMessageContent(m_encryptedVideoData, m_imageData, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes);
			}

			bool ContactEncryptedVideoAndImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_imageData, fixedNonce);
			}

			bool ContactEncryptedVideoAndImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedVideoData, m_encryptionKey, fixedNonce);
			}

			MessageContent* ContactEncryptedVideoAndImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent(m_encryptedVideoData, kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_lengthInSeconds, m_videoSizeInBytes, m_imageData.size());
				} else if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new ContactVideoMessageContent.");
					return new ContactVideoMessageContent(kfndct->getDecryptedData(), m_imageData, m_lengthInSeconds);
				} else {
					LOGGER()->critical("ContactEncryptedVideoAndImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedVideoAndImageAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& ContactEncryptedVideoAndImageAndKeyMessageContent::getImageData() const {
				return m_imageData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedVideoAndImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactEncryptedVideoAndImageAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactEncryptedVideoAndImageAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			MessageContent* ContactEncryptedVideoAndImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedVideoAndImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
