#include "src/messages/contact/file/ContactEncryptedFileAndImageIdAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/file/ContactEncryptedFileAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/contact/file/ContactFileIdAndImageIdAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedFileAndImageIdAndKeyMessageContent::ContactEncryptedFileAndImageIdAndKeyMessageContent(QByteArray const& encryptedFile, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : ContactMessageContent(), m_encryptedFileData(encryptedFile), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedFileAndImageIdAndKeyMessageContent::~ContactEncryptedFileAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedFileAndImageIdAndKeyMessageContent::clone() const {
				return new ContactEncryptedFileAndImageIdAndKeyMessageContent(m_encryptedFileData, m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool ContactEncryptedFileAndImageIdAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndImageIdAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedFileData);
			}

			bool ContactEncryptedFileAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_imageBlobId);
			}

			MessageContent* ContactEncryptedFileAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
	
					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedFileAndEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedFileAndEncryptedImageAndKeyMessageContent(m_encryptedFileData, bdct->getDownloadedBlob(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactFileIdAndImageIdAndKeyMessageContent.");
					return new ContactFileIdAndImageIdAndKeyMessageContent(buct->getBlobId(), m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedFileAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& ContactEncryptedFileAndImageIdAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 ContactEncryptedFileAndImageIdAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* ContactEncryptedFileAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndImageIdAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedFileAndImageIdAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndImageIdAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
