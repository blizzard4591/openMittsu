#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_

#include "src/messages/group/GroupMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupEncryptedImageAndKeyMessageContent : public GroupMessageContent {
			public:
				GroupEncryptedImageAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& encryptedImage, openmittsu::crypto::EncryptionKey const& encryptionKey, quint32 imageSizeInBytes);
				virtual ~GroupEncryptedImageAndKeyMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPreSendCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<openmittsu::acknowledgments::AcknowledgmentProcessor> const& acknowledgmentProcessor, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual bool hasPostReceiveCallbackTask() const override;
				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;

				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getEncryptedImageData() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint32 getImageSizeInBytes() const;
			private:
				QByteArray const encryptedImageData;
				openmittsu::crypto::EncryptionKey const encryptionKey;
				quint32 const sizeInBytes;
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
