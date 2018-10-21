#include "src/messages/contact/video/ContactVideoMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/video/ContactEncryptedVideoAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactVideoMessageContent::ContactVideoMessageContent(QByteArray const& video, QByteArray const& image, quint16 lengthInSeconds) : ContactMessageContent(), m_videoData(video), m_imageData(image), m_lengthInSeconds(lengthInSeconds) {
				// Intentionally left empty.
			}

			ContactVideoMessageContent::~ContactVideoMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactVideoMessageContent::clone() const {
				return new ContactVideoMessageContent(m_videoData, m_imageData, m_lengthInSeconds);
			}

			bool ContactVideoMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactVideoMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_videoData, fixedNonce);
			}

			MessageContent* ContactVideoMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new ContactEncryptedVideoAndImageAndKeyMessageContent.");
					return new ContactEncryptedVideoAndImageAndKeyMessageContent(kfnect->getEncryptedData(), m_imageData, kfnect->getEncryptionKey(), m_lengthInSeconds, m_videoData.size());
				} else {
					LOGGER()->critical("ContactVideoMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactVideoMessageContent::getVideoData() const {
				return m_videoData;
			}

			QByteArray const& ContactVideoMessageContent::getImageData() const {
				return m_imageData;
			}

			quint16 ContactVideoMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			MessageContent* ContactVideoMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactVideoMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactVideoMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactVideoMessageContent does not support building a packet payload.";
			}

		}
	}
}
