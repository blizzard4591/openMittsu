#include "src/messages/contact/ContactAudioMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/ContactEncryptedAudioAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

#include <QAudioBuffer>

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactAudioMessageContent::ContactAudioMessageContent(QByteArray const& audioData, quint16 lengthInSeconds) : m_audioData(audioData), m_lengthInSeconds(lengthInSeconds) {
				// Intentionally left empty.
			}

			ContactAudioMessageContent::~ContactAudioMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactAudioMessageContent::clone() const {
				return new ContactAudioMessageContent(m_audioData, m_lengthInSeconds);
			}

			bool ContactAudioMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactAudioMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_audioData, fixedNonce);
			}

			MessageContent* ContactAudioMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new ContactEncryptedAudioAndKeyMessageContent.");
					return new ContactEncryptedAudioAndKeyMessageContent(kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_lengthInSeconds, static_cast<quint32>(m_audioData.size()));
				} else {
					LOGGER()->critical("ContactAudioMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactAudioMessageContent::getAudioData() const {
				return m_audioData;
			}

			quint16 ContactAudioMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			MessageContent* ContactAudioMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactAudioMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactAudioMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactAudioMessageContent does not support building a packet payload.";
			}

		}
	}
}
