#include "src/messages/group/GroupSetPhotoMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/GroupEncryptedGroupPhotoAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/FixedNonces.h"
#include "src/crypto/Nonce.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupSetPhotoMessageContent::GroupSetPhotoMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& groupPhotoData) : GroupMessageContent(groupId), groupPhoto(groupPhotoData) {
				// Intentionally left empty.
			}

			GroupSetPhotoMessageContent::~GroupSetPhotoMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupSetPhotoMessageContent::clone() const {
				return new GroupSetPhotoMessageContent(getGroupId(), groupPhoto);
			}

			bool GroupSetPhotoMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupSetPhotoMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedImageNonce = openmittsu::crypto::FixedNonces::getFixedGroupImageNonce();
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, groupPhoto, fixedImageNonce);
			}

			MessageContent* GroupSetPhotoMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedGroupPhotoAndKeyMessageContent.");
					return new GroupEncryptedGroupPhotoAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), kfnect->getEncryptionKey(), static_cast<quint32>(groupPhoto.size()));
				} else {
					LOGGER()->critical("GroupImageMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupSetPhotoMessageContent::getGroupPhoto() const {
				return groupPhoto;
			}

			MessageContent* GroupSetPhotoMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupSetPhotoMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupSetPhotoMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupSetPhotoMessageContent does not support building a packet payload.";
			}

		}
	}
}
