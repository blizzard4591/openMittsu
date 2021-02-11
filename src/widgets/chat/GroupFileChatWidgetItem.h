#ifndef OPENMITTSU_WIDGETS_CHAT_GROUPFILECHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_CHAT_GROUPFILECHATWIDGETITEM_H_

#include "src/widgets/chat/ChatWidgetItem.h"

#include "src/dataproviders/BackedGroup.h"
#include "src/dataproviders/BackedGroupMessage.h"

#include "src/widgets/chat/GroupMediaChatWidgetItem.h"
#include "src/widgets/GifPlayer.h"

#include <QBuffer>
#include <QMovie>

#include <memory>

namespace openmittsu {
	namespace widgets {

		class GroupFileChatWidgetItem : public GroupMediaChatWidgetItem {
			Q_OBJECT
		public:
			explicit GroupFileChatWidgetItem(openmittsu::dataproviders::BackedGroupMessage const& message, QWidget* parent = nullptr);
			virtual ~GroupFileChatWidgetItem();
		public slots:
			virtual void onMessageDataChanged() override;
			virtual void onImageHasBeenClicked();
		protected:
			virtual void copyToClipboard() override;

			virtual QString getFileExtension() const override;
			virtual bool saveMediaToFile(QString const& filename) const override;
			virtual QString getDefaultFilename() const override;
		private:
			std::unique_ptr<GifPlayer> m_lblImage;
			std::unique_ptr<QLabel> m_lblCaption;
			QString m_mimeType;
			QString m_fileName;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_CHAT_GROUPFILECHATWIDGETITEM_H_
