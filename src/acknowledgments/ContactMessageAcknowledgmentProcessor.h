#ifndef OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "acknowledgments/AcknowledgmentProcessor.h"
#include "protocol/ContactId.h"
#include "protocol/MessageId.h"

class ContactMessageAcknowledgmentProcessor : public AcknowledgmentProcessor {
public:
	ContactMessageAcknowledgmentProcessor(ContactId const& receiver, QDateTime const& timeoutTime, MessageId const& messageId);
	virtual ~ContactMessageAcknowledgmentProcessor();

	virtual ContactId const& getReceiver() const;
	virtual MessageId const& getMessageId() const;

	virtual void sendFailed(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) override;
	virtual void sendSuccess(ProtocolClient* protocolClient, MessageCenter* messageCenter, MessageId const& messageId) override;

	virtual void addMessage(MessageId const& addedMessageId) override;
private:
	ContactId const receiver;
	MessageId const messageId;
};

#endif // OPENMITTSU_ACKNOWLEDGMENTS_CONTACTMESSAGEACKNOWLEDGMENTPROCESSOR_H_
