#include "src/messages/contact/image/ContactImageIdAndKeyMessageContent.h"

#include "src/utility/Endian.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/messages/MessageContentRegistry.h"
#include "src/messages/contact/image/ContactEncryptedImageAndKeyAndImageIdMessageContent.h"
#include "src/protocol/ProtocolSpecs.h"
#include "src/tasks/BlobDownloaderCallbackTask.h"
#include "src/utility/ByteArrayConversions.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			// Register this MessageContent with the MessageContentRegistry
			bool ContactImageIdAndKeyMessageContent::registrationResult = MessageContentRegistry::getInstance().registerContent(PROTO_MESSAGE_SIGNATURE_CONTACT_PICTURE, new TypedMessageContentFactory<ContactImageIdAndKeyMessageContent>());

			ContactImageIdAndKeyMessageContent::ContactImageIdAndKeyMessageContent() : imageId(), imageNonce(), sizeInBytes(0) {
				// Only accessible and used by the MessageContentFactory.
			}

			ContactImageIdAndKeyMessageContent::ContactImageIdAndKeyMessageContent(QByteArray const& imageId, openmittsu::crypto::Nonce const& imageNonce, quint32 sizeInBytes) : imageId(imageId), imageNonce(imageNonce), sizeInBytes(sizeInBytes) {
				if (imageId.size() != PROTO_IMAGESERVER_ID_LENGTH_BYTES) {
					throw openmittsu::exceptions::IllegalArgumentException() << "The supplied image ID has " << imageId.size() << " Bytes instead of " << PROTO_IMAGESERVER_ID_LENGTH_BYTES << " Bytes.";
				}
			}

			ContactImageIdAndKeyMessageContent::~ContactImageIdAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactImageIdAndKeyMessageContent::clone() const {
				return new ContactImageIdAndKeyMessageContent(imageId, imageNonce, sizeInBytes);
			}

			bool ContactImageIdAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactImageIdAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDownloaderCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), imageId);
			}

			MessageContent* ContactImageIdAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobDownloaderCallbackTask const* bdct = dynamic_cast<openmittsu::tasks::BlobDownloaderCallbackTask const*>(callbackTask);
					if (bdct->getDownloadedBlob().size() != static_cast<int>(sizeInBytes)) {
						LOGGER()->warn("Size of downloaded blob differs from stated size ({} Bytes downloaded vs. {} Bytes promised).", bdct->getDownloadedBlob().size(), sizeInBytes);
					}

					LOGGER_DEBUG("Integrating result from BlobDownloaderCallbackTask into a new ContactEncryptedImageAndKeyAndImageIdMessageContent.");
					return new ContactEncryptedImageAndKeyAndImageIdMessageContent(bdct->getDownloadedBlob(), imageNonce, sizeInBytes, imageId);
				} else {
					LOGGER()->critical("ContactImageIdAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactImageIdAndKeyMessageContent::getImageId() const {
				return imageId;
			}

			openmittsu::crypto::Nonce const& ContactImageIdAndKeyMessageContent::getImageNonce() const {
				return imageNonce;
			}

			quint32 ContactImageIdAndKeyMessageContent::getImageSizeInBytes() const {
				return sizeInBytes;
			}

			MessageContent* ContactImageIdAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				verifyPayloadMinSizeAndSignatureByte(PROTO_MESSAGE_SIGNATURE_CONTACT_PICTURE, (1 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES)+4 + openmittsu::crypto::Nonce::getNonceLength()), payload, true);

				QByteArray id(payload.mid(1, PROTO_IMAGESERVER_ID_LENGTH_BYTES));
				uint32_t size = openmittsu::utility::ByteArrayConversions::convert4ByteQByteArrayToQuint32(payload.mid(1 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES), 4));
				openmittsu::crypto::Nonce nonce(payload.mid(1 + (PROTO_IMAGESERVER_ID_LENGTH_BYTES)+4, openmittsu::crypto::Nonce::getNonceLength()));

				return new ContactImageIdAndKeyMessageContent(id, nonce, size);
			}

			QByteArray ContactImageIdAndKeyMessageContent::toPacketPayload() const {
				QByteArray result(1, PROTO_MESSAGE_SIGNATURE_CONTACT_PICTURE);
				result.append(imageId);
				result.append(openmittsu::utility::Endian::uint32FromHostToLittleEndianByteArray(sizeInBytes));
				result.append(imageNonce.getNonce());

				return result;
			}

		}
	}
}
