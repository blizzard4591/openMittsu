#include "src/messages/group/file/GroupFileMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/file/GroupEncryptedFileAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupFileMessageContent::GroupFileMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& file, QByteArray const& image, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes) : GroupMessageContent(groupId), m_fileData(file), m_imageData(image), m_mimeType(mimeType), m_fileName(fileName), m_caption(caption), m_fileSizeInBytes(fileSizeInBytes) {
				// Intentionally left empty.
			}

			GroupFileMessageContent::~GroupFileMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupFileMessageContent::clone() const {
				return new GroupFileMessageContent(getGroupId(), m_fileData, m_imageData, m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
			}

			bool GroupFileMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupFileMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_fileData, fixedNonce);
			}

			MessageContent* GroupFileMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedFileAndImageAndKeyMessageContent.");
					return new GroupEncryptedFileAndImageAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), m_imageData, kfnect->getEncryptionKey(), m_mimeType, m_fileName, m_caption, m_fileSizeInBytes);
				} else {
					LOGGER()->critical("GroupFileMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupFileMessageContent::getFileData() const {
				return m_fileData;
			}

			QByteArray const& GroupFileMessageContent::getImageData() const {
				return m_imageData;
			}

			QString const& GroupFileMessageContent::getMimeType() const {
				return m_mimeType;
			}
			
			QString const& GroupFileMessageContent::getFileName() const {
				return m_fileName;
			}
			
			QString const& GroupFileMessageContent::getCaption() const {
				return m_caption;
			}

			quint64 GroupFileMessageContent::getFileSizeInBytes() const {
				return m_fileSizeInBytes;
			}

			MessageContent* GroupFileMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupFileMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupFileMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupFileMessageContent does not support building a packet payload.";
			}

		}
	}
}
