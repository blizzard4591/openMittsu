#include "src/messages/contact/ContactEncryptedImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/ContactImageIdAndKeyMessageContent.h"
#include "src/messages/contact/ContactImageMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/SymmetricDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedImageAndKeyMessageContent::ContactEncryptedImageAndKeyMessageContent(QByteArray const& encryptedImageData, openmittsu::crypto::Nonce const& imageNonce, quint32 imageSizeInBytes) : encryptedImageData(encryptedImageData), nonce(imageNonce), sizeInBytes(imageSizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedImageAndKeyMessageContent::~ContactEncryptedImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedImageAndKeyMessageContent::clone() const {
				return new ContactEncryptedImageAndKeyMessageContent(encryptedImageData, nonce, sizeInBytes);
			}

			bool ContactEncryptedImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, encryptedImageData);
			}

			bool ContactEncryptedImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::SymmetricDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), encryptedImageData, nonce, message->getMessageHeader().getSender());
			}

			MessageContent* ContactEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::SymmetricDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::SymmetricDecryptionCallbackTask const* sdct = dynamic_cast<openmittsu::tasks::SymmetricDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from SymmetricDecryptionCallbackTask into a new ContactImageMessageContent.");
					return new ContactImageMessageContent(sdct->getDecryptedData());
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactImageIdAndKeyMessageContent.");
					return new ContactImageIdAndKeyMessageContent(buct->getBlobId(), nonce, sizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedImageAndKeyMessageContent::getEncryptedImageData() const {
				return encryptedImageData;
			}

			openmittsu::crypto::Nonce const& ContactEncryptedImageAndKeyMessageContent::getNonce() const {
				return nonce;
			}

			quint32 ContactEncryptedImageAndKeyMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			MessageContent* ContactEncryptedImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
