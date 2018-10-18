#include "src/messages/group/GroupAudioMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/GroupEncryptedAudioAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupAudioMessageContent::GroupAudioMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& audio, quint16 lengthInSeconds) : GroupMessageContent(groupId), m_audioData(audio), m_lengthInSeconds(lengthInSeconds) {
				// Intentionally left empty.
			}

			GroupAudioMessageContent::~GroupAudioMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupAudioMessageContent::clone() const {
				return new GroupAudioMessageContent(getGroupId(), m_audioData, m_lengthInSeconds);
			}

			bool GroupAudioMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupAudioMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_audioData, fixedNonce);
			}

			MessageContent* GroupAudioMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedAudioAndKeyMessageContent.");
					return new GroupEncryptedAudioAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_lengthInSeconds, static_cast<quint32>(m_audioData.size()));
				} else {
					LOGGER()->critical("GroupAudioMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupAudioMessageContent::getAudioData() const {
				return m_audioData;
			}

			quint16 GroupAudioMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			MessageContent* GroupAudioMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupAudioMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupAudioMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupAudioMessageContent does not support building a packet payload.";
			}

		}
	}
}
