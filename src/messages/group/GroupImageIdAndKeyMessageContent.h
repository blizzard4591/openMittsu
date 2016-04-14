#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/group/GroupMessageContent.h"
#include "protocol/EncryptionKey.h"
#include "protocol/GroupId.h"

#include <QByteArray>
#include <QtGlobal>

class GroupImageIdAndKeyMessageContent : public GroupMessageContent {
public:
	GroupImageIdAndKeyMessageContent(GroupId const& groupId, QByteArray const& imageId, EncryptionKey const& encryptionKey, quint32 sizeInBytes);
	virtual ~GroupImageIdAndKeyMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual bool hasPostReceiveCallbackTask() const override;

	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getImageId() const;
	EncryptionKey const& getEncryptionKey() const;
	quint32 getImageSizeInBytes() const;

	friend class TypedMessageContentFactory<GroupImageIdAndKeyMessageContent>;
private:
	QByteArray const imageId;
	EncryptionKey const encryptionKey;
	quint32 const sizeInBytes;

	static bool registrationResult;

	GroupImageIdAndKeyMessageContent();
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEIDANDKEYMESSAGECONTENT_H_
