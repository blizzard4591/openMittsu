#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPIMAGECHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPIMAGECHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupMediaChatWidgetItem.h"
#include "src/widgets/ClickAwareLabel.h"

namespace openmittsu {
	namespace widgets {

		class GroupImageChatWidgetItem : public GroupMediaChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupImageChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupImageChatWidgetItem();
		public slots:
			virtual void onMessageDataChanged() override;

			virtual void onImageHasBeenClicked();
		protected:
			virtual void copyToClipboard() override;

			virtual QString getFileExtension() const override;
			virtual bool saveMediaToFile(QString const& filename) const override;
			virtual QString getDefaultFilename() const override;
		private:
			ClickAwareLabel* m_lblImage;
			QLabel* m_lblCaption;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPIMAGECHATWIDGETITEM_H_
