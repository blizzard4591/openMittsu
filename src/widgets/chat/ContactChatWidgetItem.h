#ifndef OPENMITTSU_WIDGETS_CONTACTCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CONTACTCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include <memory>

#include "src/dataproviders/BackedContactMessage.h"
#include "src/dataproviders/BackedContact.h"

namespace openmittsu {
	namespace widgets {

		class ContactChatWidgetItem : public ChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactChatWidgetItem();
		public slots:
			virtual void showContextMenu(const QPoint& pos) override;
		protected:
			virtual openmittsu::dataproviders::BackedMessage& getMessage() override;
			virtual openmittsu::dataproviders::BackedMessage const& getMessage() const override;

			openmittsu::dataproviders::BackedContactMessage m_contactMessage;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CONTACTCHATWIDGETITEM_H_
