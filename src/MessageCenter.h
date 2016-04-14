#ifndef OPENMITTSU_MESSAGECENTER_H_
#define OPENMITTSU_MESSAGECENTER_H_

#include <QObject>
#include <QString>
#include <QHash>
#include <QSet>
#include <QByteArray>
#include <QMutex>
#include <QSharedPointer>

#include <cstdint>
#include <memory>

#include "IdentityContact.h"
#include "GroupContact.h"
#include "ServerConfiguration.h"
#include "messages/Message.h"
#include "messages/FullMessageHeader.h"
#include "messages/contact/PreliminaryContactMessage.h"
#include "messages/group/PreliminaryGroupMessage.h"
#include "protocol/SimpleUniqueMessageIdGenerator.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/ContactIdWithMessageId.h"
#include "protocol/MessageId.h"
#include "ProtocolClient.h"
#include "ChatTabWidget.h"

class ChatTab;

class MessageCenter : public QObject {
	Q_OBJECT
public:
	static MessageCenter* getInstance();

	bool registerChatTabForIdentity(ChatTab* chatTab, ContactId const& identityContactId);
	bool unregisterChatTabForIdentity(ContactId const& identityContactId);
	bool registerChatTabForGroup(ChatTab* chatTab, GroupId const& groupContactId);
	bool unregisterChatTabForGroup(GroupId const& groupContactId);

	bool sendMessageToContact(PreliminaryContactMessage const& contactMessage);
	bool sendMessageToGroup(PreliminaryGroupMessage const& groupMessage);
	
	UniqueMessageIdGenerator* getUniqueMessgeIdGenerator();

	void setProtocolClient(ProtocolClient* newProtocolClient);
	void setTabContainer(ChatTabWidget* newChatTabWidget);
	void chatTabHasNewUnreadMessageAvailable(ChatTab* self);

	ChatTab* ensureTabOpenForIdentityContact(ContactId const& identity);
	ChatTab* ensureTabOpenForGroupContact(GroupId const& groupId);

	bool hasTabOpenForIdentityContact(ContactId const& identity) const;
	bool hasTabOpenForGroupContact(GroupId const& groupId) const;

	void closeTabForIdentityContact(ContactId const& identity);
	void closeTabForGroupContact(GroupId const& groupId);
signals:
	void newUnreadMessageAvailable(ChatTab* source);
public slots:
	void onReceivedIdentityContactMessageText(FullMessageHeader header, QString message);
	void onReceivedIdentityContactMessageImage(FullMessageHeader header, QByteArray image);
	void onReceivedIdentityContactMessageLocation(FullMessageHeader header, double latitude, double longitude, double height, QString description);
	void onReceivedIdentityContactMessageReceiptReceived(FullMessageHeader header, MessageId messageId);
	void onReceivedIdentityContactMessageReceiptSeen(FullMessageHeader header, MessageId messageId);
	void onReceivedIdentityContactMessageReceiptAgree(FullMessageHeader header, MessageId messageId);
	void onReceivedIdentityContactMessageReceiptDisagree(FullMessageHeader header, MessageId messageId);
	void onReceivedIdentityContactStartedTypingNotification(ContactId sender);
	void onReceivedIdentityContactStoppedTypingNotification(ContactId sender);

	void onReceivedGroupContactMessageText(FullMessageHeader header, GroupId groupId, QString message);
	void onReceivedGroupContactMessageImage(FullMessageHeader header, GroupId groupId, QByteArray image);
	void onReceivedGroupContactMessageLocation(FullMessageHeader header, GroupId groupId, double latitude, double longitude, double height, QString description);
	void onMessageSendFailed(ContactId receiver, MessageId messageId);
	void onMessageSendDone(ContactId receiver, MessageId messageId);
	void onMessageSendFailed(GroupId group, MessageId messageId);
	void onMessageSendDone(GroupId group, MessageId messageId);

	// New Contact, new Group, group changes...
	void onFoundNewContact(ContactId newContact, PublicKey publicKey);
	void onFoundNewGroup(GroupId groupId, QSet<ContactId> members);

	void onGroupSetTitle(GroupId groupId, QString groupTitle);
	void onGroupSetImage(GroupId groupId, QByteArray image);
	void onGroupSync(GroupId groupId, QSet<ContactId> members);

	void onTimerSendGroupChatReceivedAutoReply();
private:
	MessageCenter() : protocolClient(nullptr), chatTabWidget(nullptr), accessMutex() {	}

	MessageCenter(const MessageCenter &); // hide copy constructor
	MessageCenter& operator=(const MessageCenter &); // hide assign op
	// we leave just the declarations, so the compiler will warn us 
	// if we try to use those two functions by accident

	static MessageCenter* instance;
	ProtocolClient* protocolClient;
	ChatTabWidget* chatTabWidget;
	SimpleUniqueMessageIdGenerator uniqueIdGenerator;

	mutable QMutex accessMutex;
	QHash<ContactId, ChatTab*> identityToChatTabHashMap;
	QHash<GroupId, ChatTab*> groupToChatTabHashMap;
	QHash<ContactIdWithMessageId, ChatTab*> messageIdToChatTabHashMap;
	QSet<ContactId> contactBlacklist;
};

#endif // OPENMITTSU_MESSAGECENTER_H_