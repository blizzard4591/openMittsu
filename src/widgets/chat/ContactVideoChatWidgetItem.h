#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTVIDEOCHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTVIDEOCHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactMediaChatWidgetItem.h"

#include "src/widgets/player/Player.h"

namespace openmittsu {
	namespace widgets {

		class ContactVideoChatWidgetItem : public ContactMediaChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactVideoChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactVideoChatWidgetItem();

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

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTVIDEOCHATWIDGETITEM_H_
