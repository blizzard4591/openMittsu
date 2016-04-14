#include "messages/PreliminaryMessageFactory.h"

#include "messages/contact/ContactTextMessageContent.h"
#include "messages/contact/ContactImageMessageContent.h"
#include "messages/contact/PreliminaryContactMessageHeader.h"
#include "messages/contact/ReceiptMessageContent.h"
#include "messages/contact/UserTypingMessageContent.h"

#include "messages/group/PreliminaryGroupMessageHeader.h"
#include "messages/group/GroupTextMessageContent.h"
#include "messages/group/GroupImageMessageContent.h"

#include "messages/MessageFlagsFactory.h"

PreliminaryMessageFactory::PreliminaryMessageFactory() {
	throw;
}

PreliminaryMessageFactory::PreliminaryMessageFactory(PreliminaryMessageFactory const& other) {
	throw;
}

PreliminaryMessageFactory::~PreliminaryMessageFactory() {
	// Intentionally left empty.
}

PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactTextMessage(ContactId const& receiverId, MessageId const& messageId, QString const& text) {
	return PreliminaryContactMessage(new PreliminaryContactMessageHeader(receiverId, messageId, MessageFlagsFactory::createContactMessageFlags()), new ContactTextMessageContent(text));
}

PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactImageMessage(ContactId const& receiverId, MessageId const& messageId, QByteArray const& imageData) {
	return PreliminaryContactMessage(new PreliminaryContactMessageHeader(receiverId, messageId, MessageFlagsFactory::createContactMessageFlags()), new ContactImageMessageContent(imageData));
}

PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactUserTypingStartedMessage(ContactId const& receiverId, MessageId const& messageId) {
	return PreliminaryContactMessage(new PreliminaryContactMessageHeader(receiverId, messageId, MessageFlagsFactory::createTypingStatusMessageFlags()), new UserTypingMessageContent(true));
}

PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactUserTypingStoppedMessage(ContactId const& receiverId, MessageId const& messageId) {
	return PreliminaryContactMessage(new PreliminaryContactMessageHeader(receiverId, messageId, MessageFlagsFactory::createTypingStatusMessageFlags()), new UserTypingMessageContent(false));
}

PreliminaryContactMessage PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(ContactId const& receiverId, MessageId const& messageId, MessageId const& relatedMessage, ReceiptMessageContent::ReceiptType const& receiptType) {
	return PreliminaryContactMessage(new PreliminaryContactMessageHeader(receiverId, messageId, MessageFlagsFactory::createReceiptMessageFlags()), new ReceiptMessageContent(relatedMessage, receiptType));
}

PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupTextMessage(GroupId const& groupId, MessageId const& messageId, QString const& text) {
	return PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, messageId, MessageFlagsFactory::createGroupTextMessageFlags()), new GroupTextMessageContent(groupId, text));
}

PreliminaryGroupMessage PreliminaryMessageFactory::createPreliminaryGroupImageMessage(GroupId const& groupId, MessageId const& messageId, QByteArray const& imageData) {
	return PreliminaryGroupMessage(new PreliminaryGroupMessageHeader(groupId, messageId, MessageFlagsFactory::createGroupTextMessageFlags()), new GroupImageMessageContent(groupId, imageData));
}
