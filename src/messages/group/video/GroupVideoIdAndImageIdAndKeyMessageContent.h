#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/crypto/EncryptionKey.h"
#include "src/protocol/GroupId.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupVideoIdAndImageIdAndKeyMessageContent : public GroupMessageContent {
			public:
				GroupVideoIdAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& videoBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, quint16 lengthInSeconds, quint32 videoSizeInBytes, quint32 imageSizeInBytes);
				virtual ~GroupVideoIdAndImageIdAndKeyMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getVideoBlobId() const;
				QByteArray const& getImageBlobId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				quint16 getLengthInSeconds() const;
				quint32 getVideoSizeInBytes() const;
				quint32 getImageSizeInBytes() const;

				friend class TypedMessageContentFactory<GroupVideoIdAndImageIdAndKeyMessageContent>;
			private:
				QByteArray const m_videoBlobId;
				QByteArray const m_imageBlobId;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				quint16 const m_lengthInSeconds;
				quint32 const m_videoSizeInBytes;
				quint32 const m_imageSizeInBytes;

				static bool m_registrationResult;

				GroupVideoIdAndImageIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPVIDEOIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
