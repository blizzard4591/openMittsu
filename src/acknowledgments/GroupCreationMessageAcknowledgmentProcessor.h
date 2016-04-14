#ifndef OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_
#define OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_

#include "acknowledgments/GroupContentMessageAcknowledgmentProcessor.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"

#include <QSet>

class GroupCreationMessageAcknowledgmentProcessor : public GroupContentMessageAcknowledgmentProcessor {
public:
	GroupCreationMessageAcknowledgmentProcessor(GroupId const& groupId, QDateTime const& timeoutTime, MessageId const& groupUniqueMessageId, bool informViaSignal);
	virtual ~GroupCreationMessageAcknowledgmentProcessor();
private:
	bool const informViaSignal;

	virtual void sendResultIfDone(ProtocolClient* protocolClient) override;
};

#endif // OPENMITTSU_ACKNOWLEDGMENTS_GROUPCREATIONMESSAGEACKNOWLEDGMENTPROCESSOR_H_
