#include "src/messages/contact/ContactImageMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/ContactEncryptedImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/SymmetricEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactImageMessageContent::ContactImageMessageContent(QByteArray const& imageData) : imageData(imageData) {
				// Intentionally left empty.
			}

			ContactImageMessageContent::~ContactImageMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactImageMessageContent::clone() const {
				return new ContactImageMessageContent(imageData);
			}

			bool ContactImageMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactImageMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::SymmetricEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, imageData, message->getMessageHeader().getReceiver());
			}

			MessageContent* ContactImageMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::SymmetricEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::SymmetricEncryptionCallbackTask const* sect = dynamic_cast<openmittsu::tasks::SymmetricEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from SymmetricEncryptionCallbackTask into a new ContactEncryptedImageAndKeyMessageContent.");
					return new ContactEncryptedImageAndKeyMessageContent(sect->getEncryptedData(), sect->getNonce(), static_cast<quint32>(imageData.size()));
				} else {
					LOGGER()->critical("ContactImageMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactImageMessageContent::getImageData() const {
				return imageData;
			}

			MessageContent* ContactImageMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactImageMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactImageMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactImageMessageContent does not support building a packet payload.";
			}

		}
	}
}
