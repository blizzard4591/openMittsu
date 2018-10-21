#include "src/messages/contact/video/ContactEncryptedVideoAndImageIdAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/contact/video/ContactVideoIdAndImageIdAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedVideoAndImageIdAndKeyMessageContent::ContactEncryptedVideoAndImageIdAndKeyMessageContent(QByteArray const& encryptedVideo, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : ContactMessageContent(), m_encryptedVideoData(encryptedVideo), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedVideoAndImageIdAndKeyMessageContent::~ContactEncryptedVideoAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedVideoAndImageIdAndKeyMessageContent::clone() const {
				return new ContactEncryptedVideoAndImageIdAndKeyMessageContent(m_encryptedVideoData, m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool ContactEncryptedVideoAndImageIdAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndImageIdAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedVideoData);
			}

			bool ContactEncryptedVideoAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedVideoAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_imageBlobId);
			}

			MessageContent* ContactEncryptedVideoAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_imageSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_imageSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedVideoAndEncryptedImageAndKeyMessageContent(m_encryptedVideoData, bdct->getDownloadedBlob(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactVideoIdAndImageIdAndKeyMessageContent.");
					return new ContactVideoIdAndImageIdAndKeyMessageContent(buct->getBlobId(), m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedVideoAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedVideoAndImageIdAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& ContactEncryptedVideoAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedVideoAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactEncryptedVideoAndImageIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactEncryptedVideoAndImageIdAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 ContactEncryptedVideoAndImageIdAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* ContactEncryptedVideoAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndImageIdAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedVideoAndImageIdAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedVideoAndImageIdAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
