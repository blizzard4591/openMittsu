#ifndef OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_
#define OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_

#include "messages/MessageContentFactory.h"
#include "messages/contact/ContactMessageContent.h"
#include "protocol/MessageId.h"

#include <QByteArray>

class ReceiptMessageContent : public ContactMessageContent {
public:
	enum class ReceiptType {
		RECEIVED,
		SEEN,
		AGREE,
		DISAGREE
	};

	ReceiptMessageContent(MessageId const& relatedMessage, ReceiptType const& receiptType);
	virtual ~ReceiptMessageContent();

	virtual ReceiptType const& getReceiptType() const;
	virtual MessageId const& getReferredMessageId() const;

	virtual ContactMessageContent* clone() const override;

	virtual QByteArray toPacketPayload() const override;

	virtual MessageContent* fromPacketPayload(FullMessageHeader const& messageHeader, QByteArray const& payload) const override;

	friend class TypedMessageContentFactory<ReceiptMessageContent>;
private:
	MessageId const messageId;
	ReceiptType const receiptType;

	char receiptTypeToChar(ReceiptType const& receiptType) const;
	ReceiptType charToReceiptType(char c) const;

	static bool registrationResult;

	ReceiptMessageContent();
};

#endif // OPENMITTSU_MESSAGES_CONTACT_RECEIPTMESSAGECONTENT_H_
