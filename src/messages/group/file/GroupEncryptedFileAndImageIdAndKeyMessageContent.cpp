#include "src/messages/group/file/GroupEncryptedFileAndImageIdAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/file/GroupEncryptedFileAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/group/file/GroupFileIdAndImageIdAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedFileAndImageIdAndKeyMessageContent::GroupEncryptedFileAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedFile, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : GroupMessageContent(groupId), m_encryptedFileData(encryptedFile), m_imageBlobId(imageBlobId), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedFileAndImageIdAndKeyMessageContent::~GroupEncryptedFileAndImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedFileAndImageIdAndKeyMessageContent::clone() const {
				return new GroupEncryptedFileAndImageIdAndKeyMessageContent(getGroupId(), m_encryptedFileData, m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool GroupEncryptedFileAndImageIdAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndImageIdAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedFileData);
			}

			bool GroupEncryptedFileAndImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_imageBlobId);
			}

			MessageContent* GroupEncryptedFileAndImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
	
					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new GroupEncryptedFileAndEncryptedImageAndKeyMessageContent.");
					return new GroupEncryptedFileAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedFileData, bdct->getDownloadedBlob(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupFileIdAndImageIdAndKeyMessageContent.");
					return new GroupFileIdAndImageIdAndKeyMessageContent(getGroupId(), buct->getBlobId(), m_imageBlobId, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedFileAndImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getImageBlobId() const {
				return m_imageBlobId;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& GroupEncryptedFileAndImageIdAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 GroupEncryptedFileAndImageIdAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* GroupEncryptedFileAndImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndImageIdAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedFileAndImageIdAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndImageIdAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
