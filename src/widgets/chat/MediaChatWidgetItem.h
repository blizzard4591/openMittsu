#ifndef OPENMITTSU_WIDGETS_MEDIACHATWIDGETITEM_H_
#define OPENMITTSU_WIDGETS_MEDIACHATWIDGETITEM_H_

#include <QAction>
#include <QMenu>
#include <QPoint>

namespace openmittsu {
	namespace widgets {

		class MediaChatWidgetItem {
		public:
			virtual ~MediaChatWidgetItem();
		protected:
			void media_appendCustomContextMenuEntries(QPoint const& pos, QMenu& menu);
			bool media_handleCustomContextMenuEntrySelection(QWidget* parent, QAction* selectedAction);

			virtual QString getFileExtension() const = 0;
			virtual bool saveMediaToFile(QString const& filename) const = 0;
			virtual bool saveMediaToFile(QString const& filename, QByteArray const& data) const;
		private:
			QAction* m_action;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_MEDIACHATWIDGETITEM_H_
