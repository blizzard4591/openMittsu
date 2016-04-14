#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPSETPHOTOMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPSETPHOTOMESSAGECONTENT_H_

#include "messages/group/GroupMessageContent.h"

#include <QByteArray>

class GroupSetPhotoMessageContent : public GroupMessageContent {
public:
	GroupSetPhotoMessageContent(GroupId const& groupId, QByteArray const& groupPhotoData);
	virtual ~GroupSetPhotoMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual bool hasPreSendCallbackTask() const override;

	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getGroupPhoto() const;
private:
	QByteArray groupPhoto;

	static bool registrationResult;
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEMESSAGECONTENT_H_
