#include "MessageCenter.h"

#include "exceptions/IllegalArgumentException.h"
#include "exceptions/InternalErrorException.h"
#include "utility/Logging.h"
#include "widgets/SimpleContactChatTab.h"
#include "widgets/SimpleGroupChatTab.h"
#include "ContactRegistry.h"
#include "IdentityHelper.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>

// Static Initializers
MessageCenter* MessageCenter::instance = nullptr;


MessageCenter* MessageCenter::getInstance() {
	static QMutex mutex;
	if (instance == nullptr) {
		mutex.lock();

		if (instance == nullptr) {
			instance = new MessageCenter();
		}

		mutex.unlock();
	}

	return instance;
}

void MessageCenter::chatTabHasNewUnreadMessageAvailable(ChatTab* self) {
	emit newUnreadMessageAvailable(self);
}

bool MessageCenter::registerChatTabForIdentity(ChatTab* chatTab, ContactId const& identityContactId) {
	identityToChatTabHashMap.insert(identityContactId, chatTab);
	return true;
}

bool MessageCenter::unregisterChatTabForIdentity(ContactId const& identityContactId) {
	return (identityToChatTabHashMap.remove(identityContactId) >= 1);
}

bool MessageCenter::registerChatTabForGroup(ChatTab* chatTab, GroupId const& groupContactId) {
	groupToChatTabHashMap.insert(groupContactId, chatTab);
	return true;
}

bool MessageCenter::unregisterChatTabForGroup(GroupId const& groupContactId) {
	return (groupToChatTabHashMap.remove(groupContactId) >= 1);
}

bool MessageCenter::sendMessageToContact(PreliminaryContactMessage const& contactMessage) {
	if ((this->protocolClient == nullptr) || (!this->protocolClient->getIsConnected())) {
		return false;
	}

	QMetaObject::invokeMethod(this->protocolClient, "sendContactMessage", Qt::QueuedConnection, Q_ARG(PreliminaryContactMessage, contactMessage));

	return true;
}

bool MessageCenter::sendMessageToGroup(PreliminaryGroupMessage const& groupMessage) {
	if ((this->protocolClient == nullptr) || (!this->protocolClient->getIsConnected())) {
		return false;
	}

	QMetaObject::invokeMethod(this->protocolClient, "sendGroupMessage", Qt::QueuedConnection, Q_ARG(PreliminaryGroupMessage, groupMessage));

	return true;
}

void MessageCenter::setProtocolClient(ProtocolClient* newProtocolClient) {
	this->protocolClient = newProtocolClient;
}

void MessageCenter::setTabContainer(ChatTabWidget* newChatTabWidget) {
	this->chatTabWidget = newChatTabWidget;
}

UniqueMessageIdGenerator* MessageCenter::getUniqueMessgeIdGenerator() {
	return &uniqueIdGenerator;
}

ChatTab* MessageCenter::ensureTabOpenForIdentityContact(ContactId const& identity) {
	if (chatTabWidget == nullptr) {
		return nullptr;
	} else if (identityToChatTabHashMap.contains(identity)) {
		return identityToChatTabHashMap.value(identity);
	} else {
		IdentityContact* identityContact = ContactRegistry::getInstance()->getIdentity(identity);
		if (identityContact == nullptr) {
			LOGGER()->warn("Could not find Identity Contact \"{}\" for which a ChatTab was requested.", identity.toString());
			return nullptr;
		}

		ChatTab* chatWindow = new SimpleContactChatTab(identityContact, &uniqueIdGenerator, chatTabWidget);
		chatTabWidget->addTab(chatWindow, identityContact->getContactName());
		registerChatTabForIdentity(chatWindow, identity);
		return chatWindow;
	}
}

ChatTab* MessageCenter::ensureTabOpenForGroupContact(GroupId const& groupId) {
	if (chatTabWidget == nullptr) {
		return nullptr;
	} else if (groupToChatTabHashMap.contains(groupId)) {
		return groupToChatTabHashMap.value(groupId);
	} else {
		GroupContact* groupContact = ContactRegistry::getInstance()->getGroup(groupId);
		if (groupContact == nullptr) {
			LOGGER()->warn("Could not find Group Contact \"{}\" for which a ChatTab was requested.", groupId.toString());
			return nullptr;
		}

		ChatTab* chatWindow = new SimpleGroupChatTab(groupContact, &uniqueIdGenerator, chatTabWidget);
		chatTabWidget->addTab(chatWindow, groupContact->getContactName());
		registerChatTabForGroup(chatWindow, groupId);
		return chatWindow;
	}
}

bool MessageCenter::hasTabOpenForIdentityContact(ContactId const& identity) const {
	return identityToChatTabHashMap.contains(identity);
}

bool MessageCenter::hasTabOpenForGroupContact(GroupId const& groupId) const {
	return groupToChatTabHashMap.contains(groupId);
}

void MessageCenter::closeTabForIdentityContact(ContactId const& identity) {
	if (hasTabOpenForIdentityContact(identity)) {
		ChatTab* tab = identityToChatTabHashMap.value(identity);
		unregisterChatTabForIdentity(identity);
		chatTabWidget->removeTab(chatTabWidget->indexOf(tab));
		delete tab;
	}
}

void MessageCenter::closeTabForGroupContact(GroupId const& groupId) {
	if (hasTabOpenForGroupContact(groupId)) {
		ChatTab* tab = groupToChatTabHashMap.value(groupId);
		unregisterChatTabForGroup(groupId);
		chatTabWidget->removeTab(chatTabWidget->indexOf(tab));
		delete tab;
	}
}

void MessageCenter::onReceivedIdentityContactMessageText(FullMessageHeader header, QString message) {
	LOGGER_DEBUG("MessageCenter: Received message \"{}\" from ID \"{}\".", message.toStdString(), header.getSender().toString());
	if (contactBlacklist.contains(header.getSender())) {
		LOGGER_DEBUG("Ignoring message from ID \"{}\" since the contact is blacklisted.", header.getSender().toString());
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onReceivedMessage(header.getSender(), header.getTime(), header.getMessageId(), message);
	}
}

void MessageCenter::onReceivedIdentityContactMessageImage(FullMessageHeader header, QByteArray image) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onReceivedImage(header.getSender(), header.getTime(), header.getMessageId(), image);
	}
}

void MessageCenter::onReceivedIdentityContactMessageLocation(FullMessageHeader header, double latitude, double longitude, double height, QString description) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onReceivedLocation(header.getSender(), header.getTime(), header.getMessageId(), latitude, longitude, height, description);
	}
}

void MessageCenter::onReceivedIdentityContactMessageReceiptReceived(FullMessageHeader header, MessageId messageId) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onMessageReceiptReceived(header.getSender(), header.getTime(), messageId);
	}
}

void MessageCenter::onReceivedIdentityContactMessageReceiptSeen(FullMessageHeader header, MessageId messageId) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onMessageReceiptSeen(header.getSender(), header.getTime(), messageId);
	}
}

void MessageCenter::onReceivedIdentityContactMessageReceiptAgree(FullMessageHeader header, MessageId messageId) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onMessageReceiptAgree(header.getSender(), header.getTime(), messageId);
	}
}

void MessageCenter::onReceivedIdentityContactMessageReceiptDisagree(FullMessageHeader header, MessageId messageId) {
	if (contactBlacklist.contains(header.getSender())) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(header.getSender());
	if (tab != nullptr) {
		tab->onMessageReceiptDisagree(header.getSender(), header.getTime(), messageId);
	}
}

void MessageCenter::onReceivedIdentityContactStartedTypingNotification(ContactId sender) {
	if (contactBlacklist.contains(sender)) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(sender);
	if (tab != nullptr) {
		tab->onUserStartedTypingNotification();
	}
}

void MessageCenter::onReceivedIdentityContactStoppedTypingNotification(ContactId sender) {
	if (contactBlacklist.contains(sender)) {
		return;
	}
	ChatTab* tab = ensureTabOpenForIdentityContact(sender);
	if (tab != nullptr) {
		tab->onUserStoppedTypingNotification();
	}
}

void MessageCenter::onReceivedGroupContactMessageText(FullMessageHeader header, GroupId groupId, QString message) {
	ChatTab* tab = ensureTabOpenForGroupContact(groupId);
	if (tab != nullptr) {
		tab->onReceivedMessage(header.getSender(), header.getTime(), header.getMessageId(), message);
	}
}

void MessageCenter::onReceivedGroupContactMessageImage(FullMessageHeader header, GroupId groupId, QByteArray image) {
	ChatTab* tab = ensureTabOpenForGroupContact(groupId);
	if (tab != nullptr) {
		tab->onReceivedImage(header.getSender(), header.getTime(), header.getMessageId(), image);
	}
}

void MessageCenter::onReceivedGroupContactMessageLocation(FullMessageHeader header, GroupId groupId, double latitude, double longitude, double height, QString description) {
	ChatTab* tab = ensureTabOpenForGroupContact(groupId);
	if (tab != nullptr) {
		tab->onReceivedLocation(header.getSender(), header.getTime(), header.getMessageId(), latitude, longitude, height, description);
	}
}

void MessageCenter::onMessageSendFailed(ContactId receiver, MessageId messageId) {
	ChatTab* tab = ensureTabOpenForIdentityContact(receiver);
	if (tab != nullptr) {
		tab->onMessageSendFailed(messageId);
	}
}

void MessageCenter::onMessageSendDone(ContactId receiver, MessageId messageId) {
	ChatTab* tab = ensureTabOpenForIdentityContact(receiver);
	if (tab != nullptr) {
		tab->onMessageSendDone(messageId);
	}
}

void MessageCenter::onMessageSendFailed(GroupId group, MessageId messageId) {
	ChatTab* tab = ensureTabOpenForGroupContact(group);
	if (tab != nullptr) {
		tab->onMessageSendFailed(messageId);
	}
}

void MessageCenter::onMessageSendDone(GroupId group, MessageId messageId) {
	ChatTab* tab = ensureTabOpenForGroupContact(group);
	if (tab != nullptr) {
		tab->onMessageSendDone(messageId);
	}
}

void MessageCenter::onTimerSendGroupChatReceivedAutoReply() {
	// Todo
	//emit messageWasReceived(0, QDateTime::currentDateTime().toTime_t(), messageIdWasReceived);
}

void MessageCenter::onFoundNewContact(ContactId newContact, PublicKey publicKey) {
	if (!ContactRegistry::getInstance()->hasIdentity(newContact)) {
		IdentityContact* identityContact = new IdentityContact(newContact, publicKey);
		ContactRegistry::getInstance()->addIdentity(identityContact);
	}
}

void MessageCenter::onFoundNewGroup(GroupId groupId, QSet<ContactId> members) {
	if (!ContactRegistry::getInstance()->hasGroup(groupId)) {
		GroupContact* groupContact = new GroupContact(groupId);
		groupContact->addMember(members);

		ContactRegistry::getInstance()->addGroup(groupContact);
	}
}

void MessageCenter::onGroupSetTitle(GroupId groupId, QString groupTitle) {
	ContactRegistry* cr = ContactRegistry::getInstance();
	if (cr->hasGroup(groupId)) {
		cr->getGroup(groupId)->setGroupName(groupTitle);
	} else {
		LOGGER()->warn("Received group title change for unknown group {}.", groupId.toString());
	}
}

void MessageCenter::onGroupSetImage(GroupId groupId, QByteArray image) {
	LOGGER()->warn("Ignoring group title image.");
}

void MessageCenter::onGroupSync(GroupId groupId, QSet<ContactId> members) {
	ContactRegistry* cr = ContactRegistry::getInstance();
	if (cr->hasGroup(groupId)) {
		cr->getGroup(groupId)->updateMembers(members);
	} else {
		LOGGER()->warn("Received group sync for unknown group {}.", groupId.toString());
	}
}
