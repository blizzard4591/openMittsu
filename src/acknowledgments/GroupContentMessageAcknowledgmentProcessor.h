#ifndef OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "acknowledgments/AcknowledgmentProcessor.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"

#include <QSet>

class GroupContentMessageAcknowledgmentProcessor : public AcknowledgmentProcessor {
public:
	GroupContentMessageAcknowledgmentProcessor(GroupId const& groupId, QDateTime const& timeoutTime, MessageId const& groupUniqueMessageId);
	virtual ~GroupContentMessageAcknowledgmentProcessor();

	virtual GroupId const& getGroupId() const;
	virtual MessageId const& getGroupUniqueMessageId() const;

	virtual void addMessage(MessageId const& addedMessageId) override;

	virtual void sendFailed(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) override;
	virtual void sendSuccess(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) override;
protected:
	QSet<MessageId> messages;
	bool hasFailedMessage;
private:
	GroupId const groupId;
	MessageId const groupUniqueMessageId;

	virtual void sendResultIfDone(ProtocolClient* protocolClient);
};

#endif // OPENMITTSU_ACKNOWLEDGMENTS_GROUPCONTENTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
