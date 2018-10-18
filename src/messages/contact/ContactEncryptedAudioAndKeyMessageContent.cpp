#include "src/messages/contact/ContactEncryptedAudioAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/ContactAudioIdAndKeyMessageContent.h"
#include "src/messages/contact/ContactAudioMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/EncryptionKey.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/BlobUploaderCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedAudioAndKeyMessageContent::ContactEncryptedAudioAndKeyMessageContent(QByteArray const& encryptedData, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes) : m_encryptedData(encryptedData), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_sizeInBytes(sizeInBytes) {
				// Intentionally left empty.
			}

			ContactEncryptedAudioAndKeyMessageContent::~ContactEncryptedAudioAndKeyMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedAudioAndKeyMessageContent::clone() const {
				return new ContactEncryptedAudioAndKeyMessageContent(m_encryptedData, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
			}

			bool ContactEncryptedAudioAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedAudioAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobUploaderCallbackTask(serverConfiguration, message, acknowledgmentProcessor, m_encryptedData);
			}

			bool ContactEncryptedAudioAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedAudioAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedData, m_encryptionKey, fixedNonce);
			}

			MessageContent* ContactEncryptedAudioAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new ContactAudioMessageContent.");
					return new ContactAudioMessageContent(kfndct->getDecryptedData(), m_lengthInSeconds);
				} else if (dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::BlobUploaderCallbackTask const* buct = dynamic_cast<openmittsu::tasks::BlobUploaderCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from BlobUploaderCallbackTask into a new ContactAudioIdAndKeyMessageContent.");
					return new ContactAudioIdAndKeyMessageContent(buct->getBlobId(), m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedAudioAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedAudioAndKeyMessageContent::getEncryptedData() const {
				return m_encryptedData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedAudioAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactEncryptedAudioAndKeyMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactEncryptedAudioAndKeyMessageContent::getSizeInBytes() const {
				return m_sizeInBytes;
			}

			MessageContent* ContactEncryptedAudioAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedAudioAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedAudioAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedAudioAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
