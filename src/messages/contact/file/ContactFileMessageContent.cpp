#include "src/messages/contact/file/ContactFileMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageAndKeyMessageContent.h"
#include "src/messages/contact/file/ContactEncryptedFileAndImageIdAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactFileMessageContent::ContactFileMessageContent(QByteArray const& file, QByteArray const& image, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : ContactMessageContent(), m_fileData(file), m_imageData(image), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			ContactFileMessageContent::~ContactFileMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactFileMessageContent::clone() const {
				return new ContactFileMessageContent(m_fileData, m_imageData, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool ContactFileMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactFileMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_fileData, fixedNonce);
			}

			MessageContent* ContactFileMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new ContactEncryptedFileAndImageAndKeyMessageContent.");
					if (m_imageData.isEmpty()) {
						return new ContactEncryptedFileAndImageIdAndKeyMessageContent(kfnect->getEncryptedData(), QByteArray(), kfnect->getEncryptionKey(), m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
					}
					return new ContactEncryptedFileAndImageAndKeyMessageContent(kfnect->getEncryptedData(), m_imageData, kfnect->getEncryptionKey(), m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("ContactFileMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactFileMessageContent::getFileData() const {
				return m_fileData;
			}

			QByteArray const& ContactFileMessageContent::getImageData() const {
				return m_imageData;
			}

			QString const& ContactFileMessageContent::getMimeType() const {
				return m_mimeType;
			}
			
			QString const& ContactFileMessageContent::getFileName() const {
				return m_fileName;
			}
			
			QString const& ContactFileMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 ContactFileMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* ContactFileMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactFileMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactFileMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactFileMessageContent does not support building a packet payload.";
			}

		}
	}
}
