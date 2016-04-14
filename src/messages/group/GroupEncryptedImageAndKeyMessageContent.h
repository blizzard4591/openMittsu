#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_

#include "messages/group/GroupMessageContent.h"
#include "protocol/EncryptionKey.h"

#include <QByteArray>

class GroupEncryptedImageAndKeyMessageContent : public GroupMessageContent {
public:
	GroupEncryptedImageAndKeyMessageContent(GroupId const& groupId, QByteArray const& encryptedImage, EncryptionKey const& encryptionKey, quint32 imageSizeInBytes);
	virtual ~GroupEncryptedImageAndKeyMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual bool hasPreSendCallbackTask() const override;
	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;

	virtual bool hasPostReceiveCallbackTask() const override;
	virtual CallbackTask* getPostReceiveCallbackTask(Message* message, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getEncryptedImageData() const;
	EncryptionKey const& getEncryptionKey() const;
	quint32 getImageSizeInBytes() const;
private:
	QByteArray const encryptedImageData;
	EncryptionKey const encryptionKey;
	quint32 const sizeInBytes;
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPENCRYPTEDIMAGEANDKEYMESSAGECONTENT_H_
