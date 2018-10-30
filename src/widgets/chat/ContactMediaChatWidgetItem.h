#ifndef OPENMITTSU_WIDGETS_CONTACTMEDIACHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CONTACTMEDIACHATWIDGETITEM_H_

#include "src/widgets/chat/ContactChatWidgetItem.h"
#include "src/widgets/chat/MediaChatWidgetItem.h"



namespace openmittsu {
	namespace widgets {

		class ContactMediaChatWidgetItem : public ContactChatWidgetItem, protected MediaChatWidgetItem {
			Q_OBJECT
		public:
			ContactMediaChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactMediaChatWidgetItem();
		protected:
			virtual void appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) override;
			virtual bool handleCustomContextMenuEntrySelection(QAction* selectedAction) override;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CONTACTMEDIACHATWIDGETITEM_H_
