#include "src/messages/group/video/GroupVideoMessageContent.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/messages/group/video/GroupEncryptedVideoAndImageAndKeyMessageContent.h"
#include "src/crypto/FullCryptoBox.h"
#include "src/crypto/Nonce.h"
#include "src/crypto/FixedNonces.h"
#include "src/tasks/KeyAndFixedNonceEncryptionCallbackTask.h"
#include "src/utility/Logging.h"

namespace openmittsu {
	namespace messages {
		namespace group {

			GroupVideoMessageContent::GroupVideoMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& video, QByteArray const& image, quint16 lengthInSeconds) : GroupMessageContent(groupId), m_videoData(video), m_imageData(image), m_lengthInSeconds(lengthInSeconds) {
				// Intentionally left empty.
			}

			GroupVideoMessageContent::~GroupVideoMessageContent() {
				// Intentionally left empty.
			}

			GroupMessageContent* GroupVideoMessageContent::clone() const {
				return new GroupVideoMessageContent(getGroupId(), m_videoData, m_imageData, m_lengthInSeconds);
			}

			bool GroupVideoMessageContent::hasPreSendCallbackTask() const {
				return true;
			}

			openmittsu::tasks::CallbackTask* GroupVideoMessageContent::getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const {
				openmittsu::crypto::Nonce const fixedNonce = openmittsu::crypto::FixedNonces::getFixedNonce(1);
				return new openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask(cryptoBox, message, acknowledgmentProcessor, m_videoData, fixedNonce);
			}

			MessageContent* GroupVideoMessageContent::integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const {
				if (dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask) != nullptr) {
					openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const* kfnect = dynamic_cast<openmittsu::tasks::KeyAndFixedNonceEncryptionCallbackTask const*>(callbackTask);
					LOGGER_DEBUG("Integrating result from KeyAndFixedNonceEncryptionCallbackTask into a new GroupEncryptedVideoAndImageAndKeyMessageContent.");
					return new GroupEncryptedVideoAndImageAndKeyMessageContent(getGroupId(), kfnect->getEncryptedData(), m_imageData, kfnect->getEncryptionKey(), m_lengthInSeconds, m_videoData.size());
				} else {
					LOGGER()->critical("GroupVideoMessageContent::integrateCallbackTaskResult called for unexpected CallbackTask.");
					throw;
				}
			}

			QByteArray const& GroupVideoMessageContent::getVideoData() const {
				return m_videoData;
			}

			QByteArray const& GroupVideoMessageContent::getImageData() const {
				return m_imageData;
			}

			quint16 GroupVideoMessageContent::getLengthInSeconds() const {
				return m_lengthInSeconds;
			}

			MessageContent* GroupVideoMessageContent::fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupVideoMessageContent does not support creation from a packet payload.";
			}

			QByteArray GroupVideoMessageContent::toPacketPayload() const {
				throw openmittsu::exceptions::IllegalFunctionCallException() << "The intermediate content GroupVideoMessageContent does not support building a packet payload.";
			}

		}
	}
}
