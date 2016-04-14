#ifndef OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_

#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"
#include <QDateTime>

class ProtocolClient;
class MessageCenter;

class AcknowledgmentProcessor {
public:
	AcknowledgmentProcessor(QDateTime const& timeoutTime);
	virtual ~AcknowledgmentProcessor();

	virtual QDateTime const& getTimeoutTime() const;

	virtual void sendFailed(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) = 0;
	virtual void sendSuccess(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) = 0;

	virtual void addMessage(MessageId const& addedMessageId) = 0;
protected:
	void contactMessageSendFailed(ContactId const& receiver, MessageId const& messageId, ProtocolClient* protocolClient);
	void groupMessageSendFailed(GroupId const& groupId, MessageId const& messageId, ProtocolClient* protocolClient);
	void contactMessageSendSuccess(ContactId const& receiver, MessageId const& messageId, ProtocolClient* protocolClient);
	void groupMessageSendSuccess(GroupId const& groupId, MessageId const& messageId, ProtocolClient* protocolClient);

	void emitGroupCreationFailed(GroupId const& groupId, ProtocolClient* procolClient);
	void emitGroupCreationSuccess(GroupId const& groupId, ProtocolClient* procolClient);
private:
	QDateTime const timeoutTime;
};

#endif // OPENMITTSU_ACKNOWLEDGMENTS_ACKNOWLEDGMENTPROCESSOR_H_
