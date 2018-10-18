#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPAUDIOIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPAUDIOIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/crypto/EncryptionKey.h"
#include "src/protocol/GroupId.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupAudioIdAndKeyMessageContent : public GroupMessageContent {
			public:
				GroupAudioIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& blobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 sizeInBytes);
				virtual ~GroupAudioIdAndKeyMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getBlobId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getSizeInBytes() const;

				friend class TypedMessageContentFactory<GroupAudioIdAndKeyMessageContent>;
			private:
				QByteArray const m_blobId;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_sizeInBytes;

				static bool m_registrationResult;

				GroupAudioIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPAUDIOIDANDKEYMESSAGECONTENT_H_
