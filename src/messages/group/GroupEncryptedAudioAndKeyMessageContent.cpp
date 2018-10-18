#include "src/messages/group/GroupEncryptedAudioAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/GroupAudioIdAndKeyMessageContent.h"
#include "src/messages/group/GroupAudioMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedAudioAndKeyMessageContent::GroupEncryptedAudioAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedAudio, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes) : GroupMessageContent(groupId), m_encryptedData(encryptedAudio), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_sizeInBytes(sizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedAudioAndKeyMessageContent::~GroupEncryptedAudioAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedAudioAndKeyMessageContent::clone() const {
				return new GroupEncryptedAudioAndKeyMessageContent(getGroupId(), m_encryptedData, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
			}

			bool GroupEncryptedAudioAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedAudioAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedData);
			}

			bool GroupEncryptedAudioAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedAudioAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedData, m_encryptionKey, fixedNonce);
			}

			MessageContent* GroupEncryptedAudioAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupAudioMessageContent.");
					return new GroupAudioMessageContent(getGroupId(), kfndct->getDecryptedData(), m_lengthInSeconds);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new GroupAudioIdAndKeyMessageContent.");
					return new GroupAudioIdAndKeyMessageContent(getGroupId(), buct->getBlobId(), m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedAudioAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedAudioAndKeyMessageContent::getEncryptedData() const {
				return m_encryptedData;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedAudioAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 GroupEncryptedAudioAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 GroupEncryptedAudioAndKeyMessageContent::getSizeInBytes() const {
				return m_sizeInBytes;
			}

			MessageContent* GroupEncryptedAudioAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedAudioAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedAudioAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedAudioAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
