#include "ImageViewer.h"
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

#include "utility/QObjectConnectionMacro.h"

ImageViewer::ImageViewer(QImage const& img) : ui(new Ui::ImageViewerWindow), image(img), scaleFactor(1.0) {
	ui->setupUi(this);

	this->setAttribute(Qt::WA_DeleteOnClose, true);

	// Setup connections from Menu Actions
	OPENMITTSU_CONNECT(ui->actionSave_as, triggered(), this, actionSaveAsOnClick());
	OPENMITTSU_CONNECT(ui->actionClose, triggered(), this, actionCloseOnClick());
	OPENMITTSU_CONNECT(ui->actionCopy_to_Clipboard, triggered(), this, actionCopyToClipboardOnClick());
	OPENMITTSU_CONNECT(ui->actionZoom_In, triggered(), this, actionZoomInOnClick());
	OPENMITTSU_CONNECT(ui->actionZoom_out, triggered(), this, actionZoomOutOnClick());
	OPENMITTSU_CONNECT(ui->actionReset_Zoom, triggered(), this, actionNormalSizeOnClick());
	OPENMITTSU_CONNECT(ui->actionFit_to_Window, triggered(), this, actionFitToWindowOnChange());

	ui->lblImage->setMode(PixmapLabel::Mode::TouchFromInside);
	ui->lblImage->setPixmap(QPixmap::fromImage(image));

    ui->lblImage->setBackgroundRole(QPalette::Base);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3.0 / 5.0);

	QString const message = tr("Image: %2x%3, Depth: %4").arg(image.width()).arg(image.height()).arg(image.depth());
	statusBar()->showMessage(message);
	actionFitToWindowOnChange();
}

ImageViewer::~ImageViewer() {
	delete ui;
}

bool ImageViewer::saveFile(QString const& fileName) {
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
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
    QGuiApplication::clipboard()->setImage(image);
}

void ImageViewer::actionZoomInOnClick() {
	bool fitToWindow = ui->actionFit_to_Window->isChecked();
	if (!fitToWindow) {
		scaleImage(1.25);
	}
}

void ImageViewer::actionZoomOutOnClick() {
	bool fitToWindow = ui->actionFit_to_Window->isChecked();
	if (!fitToWindow) {
		scaleImage(0.75);
	}
}

void ImageViewer::actionNormalSizeOnClick() {
	bool fitToWindow = ui->actionFit_to_Window->isChecked();
	if (!fitToWindow) {
		scaleFactor = 1.0;
		scaleImage(1.0);
	}
}

void ImageViewer::actionFitToWindowOnChange() {
    bool fitToWindow = ui->actionFit_to_Window->isChecked();
	if (fitToWindow) {
		ui->lblImage->setMode(PixmapLabel::Mode::TouchFromInside);
	} else {
		ui->lblImage->setMode(PixmapLabel::Mode::FixedSize);
	}
    ui->scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow) {
		actionNormalSizeOnClick();
	}
}

void ImageViewer::scaleImage(double factor) {
    scaleFactor *= factor;
	ui->lblImage->setScalingFactor(scaleFactor);

    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}
