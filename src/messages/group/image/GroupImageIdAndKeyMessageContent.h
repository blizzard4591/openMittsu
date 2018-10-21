#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/crypto/EncryptionKey.h"
#include "src/protocol/GroupId.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupImageIdAndKeyMessageContent : public GroupMessageContent {
			public:
				GroupImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& imageId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint32 sizeInBytes);
				virtual ~GroupImageIdAndKeyMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getImageId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint32 getImageSizeInBytes() const;

				friend class TypedMessageContentFactory<GroupImageIdAndKeyMessageContent>;
			private:
				QByteArray const imageId;
				openmittsu::crypto::EncryptionKey const encryptionKey;
				quint32 const sizeInBytes;

				static bool registrationResult;

				GroupImageIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_
