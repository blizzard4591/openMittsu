#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPVIDEOCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPVIDEOCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupMediaChatWidgetItem.h"

#include "src/widgets/player/Player.h"

namespace openmittsu {
	namespace widgets {

		class GroupVideoChatWidgetItem : public GroupMediaChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupVideoChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupVideoChatWidgetItem();

			virtual void setBackgroundColorAndPadding(QString const& cssColor, int padding) override;
		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;

			virtual QString getFileExtension() const override;
			virtual bool saveMediaToFile(QString const& filename) const override;
			virtual QString getDefaultFilename() const override;
		private:
			Player* m_player;
			QLabel* m_lblCaption;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPVIDEOCHATWIDGETITEM_H_
