#include "src/messages/contact/image/ContactEncryptedImageAndKeyAndImageIdMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/image/ContactEncryptedImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/BlobDeleterCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedImageAndKeyAndImageIdMessageContent::ContactEncryptedImageAndKeyAndImageIdMessageContent(QByteArray const& encryptedImageData, openmittsu::crypto::Nonce const& imageNonce, quint32 imageSizeInBytes, QByteArray const& blobId) : encryptedImageData(encryptedImageData), nonce(imageNonce), sizeInBytes(imageSizeInBytes), blobId(blobId) {
				// Intentionally left empty.
			}

			ContactEncryptedImageAndKeyAndImageIdMessageContent::~ContactEncryptedImageAndKeyAndImageIdMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::clone() const {
				return new ContactEncryptedImageAndKeyAndImageIdMessageContent(encryptedImageData, nonce, sizeInBytes, blobId);
			}

			bool ContactEncryptedImageAndKeyAndImageIdMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedImageAndKeyAndImageIdMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDeleterCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), blobId);
			}

			MessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDeleterCallbackTask const*>(callbackTask) != nullptr) {
					LOGGER_DEBUG("BlobDeleterCallbackTask finished, building previously downloaded blob into a new ContactEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedImageAndKeyMessageContent(encryptedImageData, nonce, sizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedImageAndKeyAndImageIdMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getEncryptedImageData() const {
				return encryptedImageData;
			}

			openmittsu::crypto::Nonce const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getNonce() const {
				return nonce;
			}

			quint32 ContactEncryptedImageAndKeyAndImageIdMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			QByteArray const& ContactEncryptedImageAndKeyAndImageIdMessageContent::getImageId() const {
				return blobId;
			}

			MessageContent* ContactEncryptedImageAndKeyAndImageIdMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyAndImageIdMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedImageAndKeyAndImageIdMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedImageAndKeyAndImageIdMessageContent does not support building a packet payload.";
			}

		}
	}
}
