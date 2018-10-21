#include "src/messages/contact/video/ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndImageIdAndKeyMessageContent.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent(QByteArray const& encryptedVideo, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : ContactMessageContent(), m_encryptedVideoData(encryptedVideo), m_encryptedImageData(encryptedImage), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::~ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::clone() const {
				return new ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent(m_encryptedVideoData, m_encryptedImageData, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedImageData);
			}

			bool ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedImageData, m_encryptionKey, fixedNonce);
			}

			MessageContent* ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new ContactEncryptedVideoAndImageAndKeyMessageContent.");
					return new ContactEncryptedVideoAndImageAndKeyMessageContent(m_encryptedVideoData, kfndct->getDecryptedData(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactEncryptedVideoAndImageIdAndKeyMessageContent.");
					return new ContactEncryptedVideoAndImageIdAndKeyMessageContent(m_encryptedVideoData, buct->getBlobId(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
				return m_encryptedImageData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
