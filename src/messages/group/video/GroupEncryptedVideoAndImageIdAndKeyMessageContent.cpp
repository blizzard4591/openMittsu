#include "src/messages/group/video/GroupEncryptedVideoAndImageIdAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/video/GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/group/video/GroupVideoIdAndImageIdAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedVideoAndImageIdAndKeyMessageContent::GroupEncryptedVideoAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedVideo, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes) : GroupMessageContent(groupId), m_encryptedVideoData(encryptedVideo), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_videoSizeInBytes(videoSizeInBytes), m_imageSizeInBytes(imageSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedVideoAndImageIdAndKeyMessageContent::~GroupEncryptedVideoAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedVideoAndImageIdAndKeyMessageContent::clone() const {
				return new GroupEncryptedVideoAndImageIdAndKeyMessageContent(getGroupId(), m_encryptedVideoData, m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
			}

			bool GroupEncryptedVideoAndImageIdAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndImageIdAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedVideoData);
			}

			bool GroupEncryptedVideoAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedVideoAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_imageBlobId);
			}

			MessageContent* GroupEncryptedVideoAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(m_imageSizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), m_imageSizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent.");
					return new GroupEncryptedVideoAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedVideoData, bdct->getDownloadedBlob(), m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupVideoIdAndImageIdAndKeyMessageContent.");
					return new GroupVideoIdAndImageIdAndKeyMessageContent(getGroupId(), buct->getBlobId(), m_imageBlobId, m_encryptionKey, m_lengthInSeconds, m_videoSizeInBytes, m_imageSizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedVideoAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedVideoAndImageIdAndKeyMessageContent::getEncryptedVideoData() const {
				return m_encryptedVideoData;
			}

			QByteArray const& GroupEncryptedVideoAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedVideoAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupEncryptedVideoAndImageIdAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupEncryptedVideoAndImageIdAndKeyMessageContent::getVideoSizeInBytes() const {
				return m_videoSizeInBytes;
			}

			quint32 GroupEncryptedVideoAndImageIdAndKeyMessageContent::getImageSizeInBytes() const {
				return m_imageSizeInBytes;
			}

			MessageContent* GroupEncryptedVideoAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndImageIdAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedVideoAndImageIdAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedVideoAndImageIdAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
