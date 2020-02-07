#include "src/messages/contact/file/ContactEncryptedFileAndEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageIdAndKeyMessageContent.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::ContactEncryptedFileAndEncryptedImageAndKeyMessageContent(QByteArray const& encryptedFile, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : ContactMessageContent(), m_encryptedFileData(encryptedFile), m_encryptedImageData(encryptedImage), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::~ContactEncryptedFileAndEncryptedImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::clone() const {
				return new ContactEncryptedFileAndEncryptedImageAndKeyMessageContent(m_encryptedFileData, m_encryptedImageData, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedImageData);
			}

			bool ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedImageData, m_encryptionKey, fixedNonce);
			}

			MessageContent* ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new ContactEncryptedFileAndImageAndKeyMessageContent.");
					return new ContactEncryptedFileAndImageAndKeyMessageContent(m_encryptedFileData, kfndct->getDecryptedData(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactEncryptedFileAndImageIdAndKeyMessageContent.");
					return new ContactEncryptedFileAndImageIdAndKeyMessageContent(m_encryptedFileData, buct->getBlobId(), m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
				return m_encryptedImageData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedFileAndEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
