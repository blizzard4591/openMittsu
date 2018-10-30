#include "src/widgets/chat/MediaChatWidgetItem.h"

#include <QFileDialog>
#include <QMessageBox>

#include "src/widgets/chat/ChatWidgetItem.h"

namespace openmittsu {
	namespace widgets {

		MediaChatWidgetItem::~MediaChatWidgetItem() {
			//
		}

		void MediaChatWidgetItem::media_appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu) {
			m_action = new QAction(QString(ChatWidgetItem::tr("Save to file...")), &menu);
			menu.addAction(m_action);
		}

		bool MediaChatWidgetItem::media_handleCustomContextMenuEntrySelection(QWidget* parent, QAction* selectedAction) {
			if ((m_action != nullptr) && (m_action == selectedAction)) {
				while (true) {
					QString fileName = QFileDialog::getSaveFileName(parent, ChatWidgetItem::tr("Save Media Item"), "", ChatWidgetItem::tr("Media Item (*.%1)").arg(getFileExtension()));
					if (!fileName.isEmpty() && !fileName.isNull()) {
						if (saveMediaToFile(fileName)) {
							return true;
						} else {
							QMessageBox::warning(parent, ChatWidgetItem::tr("Could not save Media Item"), ChatWidgetItem::tr("Unable to save media item to selected location.\nPlease check permissions and free space or try a different location!"));
						}
					} else {
						return false;
					}
				}
			} else {
				return false;
			}
		}

		bool MediaChatWidgetItem::saveMediaToFile(QString const& filename, QByteArray const& data) const {
			QFile file(filename);
			if (!file.open(QFile::WriteOnly)) {
				return false;
			}

			file.write(data);

			file.close();
			return true;
		}

	}
}
