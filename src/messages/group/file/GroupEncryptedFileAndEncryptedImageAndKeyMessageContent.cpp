#include "src/messages/group/file/GroupEncryptedFileAndEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/file/GroupEncryptedFileAndImageIdAndKeyMessageContent.h"
#include "src/messages/group/file/GroupEncryptedFileAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::GroupEncryptedFileAndEncryptedImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedFile, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : GroupMessageContent(groupId), m_encryptedFileData(encryptedFile), m_encryptedImageData(encryptedImage), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::~GroupEncryptedFileAndEncryptedImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::clone() const {
				return new GroupEncryptedFileAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedFileData, m_encryptedImageData, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedImageData);
			}

			bool GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedImageData, m_encryptionKey, fixedNonce);
			}

			MessageContent* GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupEncryptedFileAndImageAndKeyMessageContent.");
					return new GroupEncryptedFileAndImageAndKeyMessageContent(getGroupId(), m_encryptedFileData, kfndct->getDecryptedData(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupEncryptedFileAndImageIdAndKeyMessageContent.");
					return new GroupEncryptedFileAndImageIdAndKeyMessageContent(getGroupId(), m_encryptedFileData, buct->getBlobId(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
				return m_encryptedImageData;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedFileAndEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
