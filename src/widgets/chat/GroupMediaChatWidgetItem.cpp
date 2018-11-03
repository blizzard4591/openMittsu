#include "src/widgets/chat/GroupMediaChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		GroupMediaChatWidgetItem::GroupMediaChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent) : GroupChatWidgetItem(message, parent), MediaChatWidgetItem() {
			//
		}

		GroupMediaChatWidgetItem::~GroupMediaChatWidgetItem() {
			//
		}

		void GroupMediaChatWidgetItem::appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) {
			this->media_appendCustomContextMenuEntries(pos, menu);
		}

		bool GroupMediaChatWidgetItem::handleCustomContextMenuEntrySelection(QAction* selectedAction) {
			return this->media_handleCustomContextMenuEntrySelection(this, selectedAction);
		}

	}
}
