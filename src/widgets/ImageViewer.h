#ifndef OPENMITTSU_WIDGETS_IMAGEVIEWER_H_
#define OPENMITTSU_WIDGETS_IMAGEVIEWER_H_

#include <QMainWindow>
#include <QImage>
#include <QScrollBar>
#include <QString>

namespace Ui {
	class ImageViewerWindow;
}

namespace openmittsu {
	namespace widgets {

		class ImageViewer : public QMainWindow {
			Q_OBJECT
		public:
			explicit ImageViewer(QImage const& img);
			virtual ~ImageViewer();
		private slots:
			void actionSaveAsOnClick();
			void actionCloseOnClick();
			void actionCopyToClipboardOnClick();
			void actionZoomInOnClick();
			void actionZoomOutOnClick();
			void actionNormalSizeOnClick();
			void actionFitToWindowOnChange();
		private:
			Ui::ImageViewerWindow* m_ui;

			bool saveFile(QString const& fileName);
			void scaleImage(double factor);
			void adjustScrollBar(QScrollBar *scrollBar, double factor);

			QImage const m_image;
			double m_scaleFactor;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_IMAGEVIEWER_H_
