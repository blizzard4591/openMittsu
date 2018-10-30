#ifndef OPENMITTSU_WIDGETS_GROUPMEDIACHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_GROUPMEDIACHATWIDGETITEM_H_

#include "src/widgets/chat/GroupChatWidgetItem.h"
#include "src/widgets/chat/MediaChatWidgetItem.h"



namespace openmittsu {
	namespace widgets {

		class GroupMediaChatWidgetItem : public GroupChatWidgetItem, protected MediaChatWidgetItem {
			Q_OBJECT
		public:
			GroupMediaChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupMediaChatWidgetItem();
		protected:
			virtual void appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) override;
			virtual bool handleCustomContextMenuEntrySelection(QAction* selectedAction) override;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_GROUPMEDIACHATWIDGETITEM_H_
