#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactMediaChatWidgetItem.h"
#include "src/widgets/GifPlayer.h"

#include <QBuffer>
#include <QMovie>

namespace openmittsu {
	namespace widgets {

		class ContactFileChatWidgetItem : public ContactMediaChatWidgetItem {
			Q_OBJECT
		public:
			explicit ContactFileChatWidgetItem(openmittsu::dataproviders::BackedContactMessage const& message, QWidget* parent = nullptr);
			virtual ~ContactFileChatWidgetItem();
		public slots:
			virtual void onMessageDataChanged() override;
		protected:
			virtual void copyToClipboard() override;

			virtual QString getFileExtension() const override;
			virtual bool saveMediaToFile(QString const& filename) const override;
		private:
			GifPlayer* m_lblImage;
			QLabel* m_lblCaption;
			QString m_mimeType;
			QString m_fileName;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_
