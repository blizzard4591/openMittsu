#include "widgets/SimpleContactChatTab.h"

#include "MessageCenter.h"
#include "protocol/ContactId.h"
#include "messages/PreliminaryMessageFactory.h"

SimpleContactChatTab::SimpleContactChatTab(IdentityContact* contact, UniqueMessageIdGenerator* idGenerator, QWidget* parent) : SimpleChatTab(contact, idGenerator, parent), identityContact(contact) {
	//
}

SimpleContactChatTab::~SimpleContactChatTab() {
	//
}

bool SimpleContactChatTab::sendText(MessageId const& uniqueMessageId, QString const& text) {
	MessageCenter::getInstance()->sendMessageToContact(PreliminaryMessageFactory::createPreliminaryContactTextMessage(identityContact->getContactId(), uniqueMessageId, text));

	return true;
}

bool SimpleContactChatTab::sendImage(MessageId const& uniqueMessageId, QByteArray const& image) {
	MessageCenter::getInstance()->sendMessageToContact(PreliminaryMessageFactory::createPreliminaryContactImageMessage(identityContact->getContactId(), uniqueMessageId, image));

	return true;
}

bool SimpleContactChatTab::sendUserTypingStatus(bool isTyping) {
	if (isTyping) {
		MessageCenter::getInstance()->sendMessageToContact(PreliminaryMessageFactory::createPreliminaryContactUserTypingStartedMessage(identityContact->getContactId(), getUniqueMessageId()));
	} else {
		MessageCenter::getInstance()->sendMessageToContact(PreliminaryMessageFactory::createPreliminaryContactUserTypingStoppedMessage(identityContact->getContactId(), getUniqueMessageId()));
	}

	return true;
}

bool SimpleContactChatTab::sendReceipt(MessageId const& receiptedMessageId, ReceiptMessageContent::ReceiptType const& receiptType) {
	MessageCenter::getInstance()->sendMessageToContact(PreliminaryMessageFactory::createPreliminaryContactMessageReceipt(identityContact->getContactId(), getUniqueMessageId(), receiptedMessageId, receiptType));

	return true;
}

MessageId SimpleContactChatTab::getUniqueMessageId() {
	return uniqueMessageIdGenerator->getNextUniqueMessageId(identityContact->getContactId());
}
