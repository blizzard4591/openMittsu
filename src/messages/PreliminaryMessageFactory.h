#ifndef OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_
#define OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_

#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"
#include "messages/contact/PreliminaryContactMessage.h"
#include "messages/group/PreliminaryGroupMessage.h"
#include "messages/contact/ReceiptMessageContent.h"

#include <QByteArray>
#include <QString>

class PreliminaryMessageFactory {
public:
	// Contact
	static PreliminaryContactMessage createPreliminaryContactTextMessage(ContactId const& receiverId, MessageId const& messageId, QString const& text);
	static PreliminaryContactMessage createPreliminaryContactImageMessage(ContactId const& receiverId, MessageId const& messageId, QByteArray const& imageData);

	static PreliminaryContactMessage createPreliminaryContactUserTypingStartedMessage(ContactId const& receiverId, MessageId const& messageId);
	static PreliminaryContactMessage createPreliminaryContactUserTypingStoppedMessage(ContactId const& receiverId, MessageId const& messageId);

	static PreliminaryContactMessage createPreliminaryContactMessageReceipt(ContactId const& receiverId, MessageId const& messageId, MessageId const& relatedMessage, ReceiptMessageContent::ReceiptType const& receiptType);

	// Groups
	static PreliminaryGroupMessage createPreliminaryGroupTextMessage(GroupId const& groupId, MessageId const& messageId, QString const& text);
	static PreliminaryGroupMessage createPreliminaryGroupImageMessage(GroupId const& groupId, MessageId const& messageId, QByteArray const& imageData);
private:
	// Disable the constructors
	PreliminaryMessageFactory();
	PreliminaryMessageFactory(PreliminaryMessageFactory const& other);
	virtual ~PreliminaryMessageFactory();
};

#endif // OPENMITTSU_MESSAGES_PRELIMINARYMESSAGEFACTORY_H_
