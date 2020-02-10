#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPFILEIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPFILEIDANDIMAGEIDANDKEYMESSAGECONTENT_H_

#include "src/messages/MessageContentFactory.h"
#include "src/messages/group/GroupMessageContent.h"
#include "src/crypto/EncryptionKey.h"

#include <QByteArray>
#include <QtGlobal>

namespace openmittsu {
	namespace messages {
		namespace group {

			class GroupFileIdAndImageIdAndKeyMessageContent : public GroupMessageContent {
			public:
				GroupFileIdAndImageIdAndKeyMessageContent(openmittsu::protocol::GroupId const& groupId, QByteArray const& fileBlobId, QByteArray const& imageBlobId, openmittsu::crypto::EncryptionKey const& encryptionKey, QString const& mimeType, QString const& fileName, QString const& caption, quint64 fileSizeInBytes);
				virtual ~GroupFileIdAndImageIdAndKeyMessageContent();

				virtual GroupMessageContent* clone() const override;

				virtual bool hasPostReceiveCallbackTask() const override;

				virtual openmittsu::tasks::CallbackTask* getPostReceiveCallbackTask(Message* message, std::shared_ptr<openmittsu::network::ServerConfiguration> const& serverConfiguration, std::shared_ptr<openmittsu::crypto::FullCryptoBox> const& cryptoBox) const override;
				virtual MessageContent* integrateCallbackTaskResult(openmittsu::tasks::CallbackTask const* callbackTask) const override;

				virtual QByteArray toPacketPayload() const override;
				virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

				QByteArray const& getFileBlobId() const;
				QByteArray const& getImageBlobId() const;
				openmittsu::crypto::EncryptionKey const& getEncryptionKey() const;
				QString const& getMimeType() const;
				QString const& getFileName() const;
				QString const& getCaption() const;
				quint64 getFileSizeInBytes() const;

				friend class TypedMessageContentFactory<GroupFileIdAndImageIdAndKeyMessageContent>;
			private:
				QByteArray const m_fileBlobId;
				QByteArray const m_imageBlobId;
				openmittsu::crypto::EncryptionKey const m_encryptionKey;
				QString const m_mimeType;
				QString const m_fileName;
				QString const m_caption;
				quint64 const m_fileSizeInBytes;

				static bool m_registrationResult;

				GroupFileIdAndImageIdAndKeyMessageContent();
			};

		}
	}
}

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPFILEIDANDIMAGEIDANDKEYMESSAGECONTENT_H_
