#include "src/messages/contact/file/ContactEncryptedFileAndImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/file/ContactEncryptedFileAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/contact/file/ContactFileMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedFileAndImageAndKeyMessageContent::ContactEncryptedFileAndImageAndKeyMessageContent(QByteArray const& encryptedFile, QByteArray const& imageData, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : ContactMessageContent(), m_encryptedFileData(encryptedFile), m_imageData(imageData), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedFileAndImageAndKeyMessageContent::~ContactEncryptedFileAndImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedFileAndImageAndKeyMessageContent::clone() const {
				return new ContactEncryptedFileAndImageAndKeyMessageContent(m_encryptedFileData, m_imageData, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool ContactEncryptedFileAndImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_imageData, fixedNonce);
			}

			bool ContactEncryptedFileAndImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedFileAndImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedFileData, m_encryptionKey, fixedNonce);
			}

			MessageContent* ContactEncryptedFileAndImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new ContactEncryptedFileAndEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedFileAndEncryptedImageAndKeyMessageContent(m_encryptedFileData, kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new ContactFileMessageContent.");
					return new ContactFileMessageContent(kfndct->getDecryptedData(), m_imageData, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedFileAndImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedFileAndImageAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& ContactEncryptedFileAndImageAndKeyMessageContent::getImageData() const {
				return m_imageData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedFileAndImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& ContactEncryptedFileAndImageAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& ContactEncryptedFileAndImageAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& ContactEncryptedFileAndImageAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 ContactEncryptedFileAndImageAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* ContactEncryptedFileAndImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedFileAndImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedFileAndImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
