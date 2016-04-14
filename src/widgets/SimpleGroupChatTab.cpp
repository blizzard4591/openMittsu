#include "widgets/SimpleGroupChatTab.h"

#include "MessageCenter.h"
#include "protocol/GroupId.h"
#include "messages/PreliminaryMessageFactory.h"

SimpleGroupChatTab::SimpleGroupChatTab(GroupContact* contact, UniqueMessageIdGenerator* idGenerator, QWidget* parent) : SimpleChatTab(contact, idGenerator, parent), groupContact(contact) {
	//
}

SimpleGroupChatTab::~SimpleGroupChatTab() {
	//
}

bool SimpleGroupChatTab::sendText(MessageId const& uniqueMessageId, QString const& text) {
	MessageCenter::getInstance()->sendMessageToGroup(PreliminaryMessageFactory::createPreliminaryGroupTextMessage(groupContact->getGroupId(), uniqueMessageId, text));

	return true;
}

bool SimpleGroupChatTab::sendImage(MessageId const& uniqueMessageId, QByteArray const& image) {
	MessageCenter::getInstance()->sendMessageToGroup(PreliminaryMessageFactory::createPreliminaryGroupImageMessage(groupContact->getGroupId(), uniqueMessageId, image));

	return true;
}

bool SimpleGroupChatTab::sendUserTypingStatus(bool isTyping) {
	return true;
}

bool SimpleGroupChatTab::sendReceipt(MessageId const& receiptedMessageId, ReceiptMessageContent::ReceiptType const& receiptType) {
	return true;
}

MessageId SimpleGroupChatTab::getUniqueMessageId() {
	return uniqueMessageIdGenerator->getNextUniqueMessageId(groupContact->getGroupId());
}
