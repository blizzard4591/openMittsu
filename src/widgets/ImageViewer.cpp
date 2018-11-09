#include "src/widgets/ImageViewer.h"
#include "ui_ImageViewer.h"

#include <QtWidgets>

// Qt 5.4 and later define QByteArrayList.
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#include <QByteArrayList>
#else
#include <QList>
#include <QByteArray>
typedef QList<QByteArray> QByteArrayList;
#endif

#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

namespace openmittsu {
	namespace widgets {

		ImageViewer::ImageViewer(QImage const& img) : m_ui(std::make_unique<Ui::ImageViewerWindow>()), m_image(img), m_scaleFactor(1.0) {
			m_ui->setupUi(this);

			this->setAttribute(Qt::WA_DeleteOnClose, true);

			// Setup connections from Menu Actions
			OPENMITTSU_CONNECT(m_ui->actionSave_as, triggered(), this, actionSaveAsOnClick());
			OPENMITTSU_CONNECT(m_ui->actionClose, triggered(), this, actionCloseOnClick());
			OPENMITTSU_CONNECT(m_ui->actionCopy_to_Clipboard, triggered(), this, actionCopyToClipboardOnClick());
			OPENMITTSU_CONNECT(m_ui->actionZoom_In, triggered(), this, actionZoomInOnClick());
			OPENMITTSU_CONNECT(m_ui->actionZoom_out, triggered(), this, actionZoomOutOnClick());
			OPENMITTSU_CONNECT(m_ui->actionReset_Zoom, triggered(), this, actionNormalSizeOnClick());
			OPENMITTSU_CONNECT(m_ui->actionFit_to_Window, triggered(), this, actionFitToWindowOnChange());

			m_ui->lblImage->setMode(PixmapLabel::Mode::TouchFromInside);
			m_ui->lblImage->setPixmap(QPixmap::fromImage(m_image));

			m_ui->lblImage->setBackgroundRole(QPalette::Base);
			m_ui->scrollArea->setBackgroundRole(QPalette::Dark);

			resize(QGuiApplication::primaryScreen()->availableSize() * 3.0 / 5.0);

			QString const message = tr("Image: %2x%3, Depth: %4").arg(m_image.width()).arg(m_image.height()).arg(m_image.depth());
			statusBar()->showMessage(message);

			m_ui->actionFit_to_Window->setChecked(true);
			actionFitToWindowOnChange();
		}

		ImageViewer::~ImageViewer() {
			//
		}

		bool ImageViewer::saveFile(QString const& fileName) {
			QImageWriter writer(fileName);

			if (!writer.write(m_image)) {
				QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("Error while saving image.\nCannot write file \"%1\": %2").arg(QDir::toNativeSeparators(fileName)), writer.errorString());
				return false;
			}
			return true;
		}

		void ImageViewer::actionSaveAsOnClick() {
			QFileDialog dialog(this, tr("Save File As"));
			QStringList mimeTypeFilters;
			QByteArrayList const supportedMimeTypes = QImageWriter::supportedMimeTypes();
			foreach(QByteArray const& mimeTypeName, supportedMimeTypes) {
				mimeTypeFilters.append(mimeTypeName);
			}
			mimeTypeFilters.sort();

			dialog.setMimeTypeFilters(mimeTypeFilters);
			dialog.selectMimeTypeFilter("image/jpeg");
			dialog.setDefaultSuffix("jpg");

			while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
		}

		void ImageViewer::actionCloseOnClick() {
			this->close();
		}

		void ImageViewer::actionCopyToClipboardOnClick() {
			QGuiApplication::clipboard()->setImage(m_image);
		}

		void ImageViewer::actionZoomInOnClick() {
			bool fitToWindow = m_ui->actionFit_to_Window->isChecked();
			if (!fitToWindow) {
				scaleImage(1.25);
			}
		}

		void ImageViewer::actionZoomOutOnClick() {
			bool fitToWindow = m_ui->actionFit_to_Window->isChecked();
			if (!fitToWindow) {
				scaleImage(0.75);
			}
		}

		void ImageViewer::actionNormalSizeOnClick() {
			bool fitToWindow = m_ui->actionFit_to_Window->isChecked();
			if (!fitToWindow) {
				m_scaleFactor = 1.0;
				scaleImage(1.0);
			} else {
				QSize const requiredSize = m_image.size();
				QSize const availableSize = QGuiApplication::primaryScreen()->availableSize();
				if ((requiredSize.width() > availableSize.width()) || (requiredSize.height() > availableSize.height())) {
					this->resize(availableSize);
				} else {
					this->resize(requiredSize);
				}
			}
		}

		void ImageViewer::actionFitToWindowOnChange() {
			bool fitToWindow = m_ui->actionFit_to_Window->isChecked();
			if (fitToWindow) {
				m_ui->lblImage->setMode(PixmapLabel::Mode::TouchFromInside);
			} else {
				m_ui->lblImage->setMode(PixmapLabel::Mode::FixedSize);
			}
			m_ui->scrollArea->setWidgetResizable(fitToWindow);
			if (!fitToWindow) {
				actionNormalSizeOnClick();
			}
		}

		void ImageViewer::scaleImage(double factor) {
			m_scaleFactor *= factor;
			m_ui->lblImage->setScalingFactor(m_scaleFactor);

			adjustScrollBar(m_ui->scrollArea->horizontalScrollBar(), factor);
			adjustScrollBar(m_ui->scrollArea->verticalScrollBar(), factor);
		}

		void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor) {
			scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
		}

	}
}

