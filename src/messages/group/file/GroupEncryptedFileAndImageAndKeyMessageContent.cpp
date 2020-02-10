#include "src/messages/group/file/GroupEncryptedFileAndImageAndKeyMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/file/GroupEncryptedFileAndEncryptedImageAndKeyMessageContent.h"
#include "src/messages/group/file/GroupFileMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/tasks/KeyAndFixedNonceDecryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupEncryptedFileAndImageAndKeyMessageContent::GroupEncryptedFileAndImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedFile, QByteArray const& imageData, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : GroupMessageContent(groupId), m_encryptedFileData(encryptedFile), m_imageData(imageData), m_encryptionKey(encryptionKey), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			GroupEncryptedFileAndImageAndKeyMessageContent::~GroupEncryptedFileAndImageAndKeyMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupEncryptedFileAndImageAndKeyMessageContent::clone() const {
				return new GroupEncryptedFileAndImageAndKeyMessageContent(getGroupId(), m_encryptedFileData, m_imageData, m_encryptionKey, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool GroupEncryptedFileAndImageAndKeyMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndImageAndKeyMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(2);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_imageData, fixedNonce);
			}

			bool GroupEncryptedFileAndImageAndKeyMessageContent::hasPostReceiveCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupEncryptedFileAndImageAndKeyMessageContent::getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask(cryptoBox, message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor>(), m_encryptedFileData, m_encryptionKey, fixedNonce);
			}

			MessageContent* GroupEncryptedFileAndImageAndKeyMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedFileAndEncryptedImageAndKeyMessageContent.");
					return new GroupEncryptedFileAndEncryptedImageAndKeyMessageContent(getGroupId(), m_encryptedFileData, kfnect->getEncryptedData(), kfnect->getEncryptionKey(), m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const* kfndct = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceDecryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceDecryptionCallbackTask into a new GroupFileMessageContent.");
					return new GroupFileMessageContent(getGroupId(), kfndct->getDecryptedData(), m_imageData, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("GroupEncryptedFileAndImageAndKeyMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupEncryptedFileAndImageAndKeyMessageContent::getEncryptedFileData() const {
				return m_encryptedFileData;
			}

			QByteArray const& GroupEncryptedFileAndImageAndKeyMessageContent::getImageData() const {
				return m_imageData;
			}

			openmittsu::crypto::EncryptionKey const& GroupEncryptedFileAndImageAndKeyMessageContent::getEncryptionKey() const {
				return m_encryptionKey;
			}

			QString const& GroupEncryptedFileAndImageAndKeyMessageContent::getMimeType() const {
				return m_mimeType;
			}

			QString const& GroupEncryptedFileAndImageAndKeyMessageContent::getFileName() const {
				return m_fileName;
			}

			QString const& GroupEncryptedFileAndImageAndKeyMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 GroupEncryptedFileAndImageAndKeyMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* GroupEncryptedFileAndImageAndKeyMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndImageAndKeyMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupEncryptedFileAndImageAndKeyMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupEncryptedFileAndImageAndKeyMessageContent does not support building a packet payload.";
			}

		}
	}
}
