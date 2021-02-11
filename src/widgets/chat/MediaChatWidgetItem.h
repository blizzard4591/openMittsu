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

			enum class LabelType {
				INVALID, GIF, IMAGE, FILE
			};

			static LabelType extractDataFromJson(QString const& text, QString& mimeType, QString& fileName, QString& fileSize);
			virtual QString getDefaultFilename() const = 0;
		private:
			QAction* m_action;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_MEDIACHATWIDGETITEM_H_
