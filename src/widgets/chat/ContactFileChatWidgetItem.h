#ifndef OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedContact.h"
#include "src/dataproviders/BackedContactMessage.h"

#include "src/widgets/chat/ContactMediaChatWidgetItem.h"
#include "src/widgets/ClickAwareLabel.h"

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

			virtual void onImageHasBeenClicked();
		protected:
			virtual void copyToClipboard() override;

			virtual QString getFileExtension() const override;
			virtual bool saveMediaToFile(QString const& filename) const override;
		private:
			ClickAwareLabel* m_lblImage;
			QLabel* m_lblCaption;
			QByteArray m_mainData;
			QByteArray m_thumbnailData;
			QBuffer m_mainDataBuffer;
			QMovie m_mainMovie;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_CONTACTFILECHATWIDGETITEM_H_
