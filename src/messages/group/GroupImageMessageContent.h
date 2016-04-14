#ifndef OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEMESSAGECONTENT_H_

#include "messages/group/GroupMessageContent.h"

#include <QByteArray>

class GroupImageMessageContent : public GroupMessageContent {
public:
	GroupImageMessageContent(GroupId const& groupId, QByteArray const& imageData);
	virtual ~GroupImageMessageContent();

	virtual GroupMessageContent* clone() const override;

	virtual bool hasPreSendCallbackTask() const override;

	virtual CallbackTask* getPreSendCallbackTask(Message* message, std::shared_ptr<AcknowledgmentProcessor> const& acknowledgmentProcessor, ServerConfiguration* serverConfiguration, CryptoBox* cryptoBox) const override;
	virtual MessageContent* integrateCallbackTaskResult(CallbackTask const* callbackTask) const override;

	virtual QByteArray toPacketPayload() const override;
	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	QByteArray const& getImageData() const;
private:
	QByteArray imageData;

	static bool registrationResult;
};

#endif // OPENMITTSU_MESSAGES_GROUP_GROUPIMAGEMESSAGECONTENT_H_
