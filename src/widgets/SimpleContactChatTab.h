#ifndef OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_

#include "widgets/SimpleChatTab.h"
#include "IdentityContact.h"

namespace Ui {
	class SimpleContactChatTab;
}

class SimpleContactChatTab : public SimpleChatTab {
	Q_OBJECT
public:
	explicit SimpleContactChatTab(IdentityContact* contact, UniqueMessageIdGenerator* idGenerator, QWidget *parent = 0);
	virtual ~SimpleContactChatTab();
protected:
	virtual bool sendText(MessageId const& uniqueMessageId, QString const& text) override;
	virtual bool sendImage(MessageId const& uniqueMessageId, QByteArray const& image) override;
	virtual bool sendUserTypingStatus(bool isTyping) override;
	virtual bool sendReceipt(MessageId const& receiptedMessageId, ReceiptMessageContent::ReceiptType const& receiptType) override;
	virtual MessageId getUniqueMessageId() override;
private:
	IdentityContact* identityContact;
};

#endif // OPENMITTSU_WIDGETS_SIMPLECONTACTCHATTAB_H_
