#ifndef OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_
#define OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_

#include "widgets/SimpleChatTab.h"
#include "GroupContact.h"

namespace Ui {
class SimpleGroupChatTab;
}

class SimpleGroupChatTab : public SimpleChatTab {
	Q_OBJECT
public:
	explicit SimpleGroupChatTab(GroupContact* contact, UniqueMessageIdGenerator* idGenerator, QWidget *parent = 0);
    virtual ~SimpleGroupChatTab();
protected:
	virtual bool sendText(MessageId const& uniqueMessageId, QString const& text) override;
	virtual bool sendImage(MessageId const& uniqueMessageId, QByteArray const& image) override;
	virtual bool sendUserTypingStatus(bool isTyping) override;
	virtual bool sendReceipt(MessageId const& receiptedMessageId, ReceiptMessageContent::ReceiptType const& receiptType) override;
	virtual MessageId getUniqueMessageId() override;
private:
	GroupContact* groupContact;
};

#endif // OPENMITTSU_WIDGETS_SIMPLEGROUPCHATTAB_H_
