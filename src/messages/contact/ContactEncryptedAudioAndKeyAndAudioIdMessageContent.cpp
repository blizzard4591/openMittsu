#include "src/messages/contact/ContactEncryptedAudioAndKeyAndAudioIdMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/contact/ContactEncryptedAudioAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/EncryptionKey.h"
#include "src/tasks/BlobDeleterCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace contact {

			ContactEncryptedAudioAndKeyAndAudioIdMessageContent::ContactEncryptedAudioAndKeyAndAudioIdMessageContent(QByteArray const& encryptedData, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes, QByteArray const& blobId) : m_encryptedData(encryptedData), m_encryptionKey(encryptionKey), m_lengthInSeconds(lengthInSeconds), m_sizeInBytes(sizeInBytes), m_blobId(blobId) {
				// Intentionally left empty.
			}

			ContactEncryptedAudioAndKeyAndAudioIdMessageContent::~ContactEncryptedAudioAndKeyAndAudioIdMessageContent() {
				// Intentionally left empty.
			}

			ContactMessageContent* ContactEncryptedAudioAndKeyAndAudioIdMessageContent::clone() const {
				return new ContactEncryptedAudioAndKeyAndAudioIdMessageContent(m_encryptedData, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes, m_blobId);
			}

			bool ContactEncryptedAudioAndKeyAndAudioIdMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				return new openmittsu::tasks::BlobDeleterCallbackTask(serverConfiguration, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_blobId);
			}

			MessageContent* ContactEncryptedAudioAndKeyAndAudioIdMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::BlobDeleterCallbackTask const*>(callbackTask) != nullptr) {
					LOGGER_DEBUG("BlobDeleterCallbackTask finished, building previously downloaded blob into a new ContactEncryptedAudioAndKeyMessageContent.");
					return new ContactEncryptedAudioAndKeyMessageContent(m_encryptedData, m_encryptionKey, m_lengthInSeconds, m_sizeInBytes);
				} else {
					LOGGER()->critical("ContactEncryptedAudioAndKeyAndAudioIdMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getEncryptedData() const {
				return m_encryptedData;
			}

			openmittsu::crypto::EncryptionKey const& ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			quint16 ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			quint32 ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getSizeInBytes() const {
				return m_sizeInBytes;
			}

			QByteArray const& ContactEncryptedAudioAndKeyAndAudioIdMessageContent::getBlobId() const {
				return m_blobId;
			}

			MessageContent* ContactEncryptedAudioAndKeyAndAudioIdMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedAudioAndKeyAndAudioIdMessageContent does not support creation from a packet payload.";
			}

			QByteArray ContactEncryptedAudioAndKeyAndAudioIdMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content ContactEncryptedAudioAndKeyAndAudioIdMessageContent does not support building a packet payload.";
			}

		}
	}
}
