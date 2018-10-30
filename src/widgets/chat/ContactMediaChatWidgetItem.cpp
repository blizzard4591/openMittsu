#include "src/widgets/chat/ContactMediaChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		ContactMediaChatWidgetItem::ContactMediaChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent) : ContactChatWidgetItem(message, parent), MediaChatWidgetItem() {
			//
		}

		ContactMediaChatWidgetItem::~ContactMediaChatWidgetItem() {
			//
		}

		void ContactMediaChatWidgetItem::appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) {
			this->media_appendCustomContextMenuEntries(pos, menu);
		}

		bool ContactMediaChatWidgetItem::handleCustomContextMenuEntrySelection(QAction* selectedAction) {
			return this->media_handleCustomContextMenuEntrySelection(this, selectedAction);
		}

	}
}
