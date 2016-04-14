#include "widgets/QrWidget.h"

#include <QPainter>
#include <QImage>
#include <QPixmap>

#include "Config.h"
#include "utility/Logging.h"

#ifdef OPENMITTSU_CONFIG_HAVE_LIBQRENCODE
#	include "qrencode.h"
#endif

QrWidget::QrWidget(QWidget *parent) : QWidget(parent), data("-EMPTY-"), qrCodeImageData(nullptr), qrCodeImageWidth(0) {
    //
}

QrWidget::~QrWidget() {
	if (qrCodeImageData != nullptr) {
		delete qrCodeImageData;
		qrCodeImageData = nullptr;
		qrCodeImageWidth = 0;
	}
}

void QrWidget::setQrDataString(QString const& newDataString) {
	data = newDataString;

	int newWidth = 0;
	char* newData = nullptr;
#ifdef OPENMITTSU_CONFIG_HAVE_LIBQRENCODE
	QRcode *qr = QRcode_encodeString(newDataString.toStdString().c_str(), 1, QR_ECLEVEL_L, QR_MODE_8, 0);

	if (qr != nullptr) {
		newWidth = qr->width;
		newData = new char[newWidth * newWidth]();

		if (newData == nullptr) {
			newWidth = 0;
			LOGGER()->warn("Failed to generate a QR code for input {}.", newDataString.toStdString());
		} else {
			LOGGER_DEBUG("Generated new QR code with {} Bits ({} x {} pixel).", newWidth * newWidth, newWidth, newWidth);
			for (int h = 0; h < newWidth; ++h) {
				for (int w = 0; w < newWidth; ++w) {
					int const offset = h * newWidth;

					newData[offset + w] = ((qr->data[offset + w] & 0x01) != 0 ? 1 : 0);
				}
			}
		}
		// Free the QR Code struct
		QRcode_free(qr);
	}
#endif

	std::swap(qrCodeImageData, newData);
	qrCodeImageWidth = newWidth;

	if (newData != nullptr) {
		delete newData;
	}

	update();
}

void QrWidget::paintEvent(QPaintEvent* paintEvent) {
	QPainter painter(this);

	if (qrCodeImageWidth <= 0) {
		painter.fillRect(0, 0, this->width(), this->height(), Qt::GlobalColor::white);
		const QRect rectangle = QRect(0, 0, this->width(), this->height());
#ifndef OPENMITTSU_CONFIG_HAVE_LIBQRENCODE
		painter.drawText(rectangle, Qt::AlignCenter, "Compiled without support for LibQREncode.");
#else
		painter.drawText(rectangle, Qt::AlignCenter, "Error while building QR code via LibQREncode.");
#endif
	} else {
		const int areaWidth = this->width();
		const int areaHeight = this->height();
		painter.fillRect(0, 0, areaWidth, areaHeight, Qt::GlobalColor::white);

		// Get the shorter side
		int shortestSize = ((areaWidth > areaHeight) ? areaHeight : areaWidth);

		// Only display if the QR Code fits in 1:1 resolution
		if (qrCodeImageWidth <= shortestSize) {
			//const double scale = ((double)shortestSize / ((double)qr->width));
			int const scale = shortestSize / qrCodeImageWidth;
			for (int h = 0; h < qrCodeImageWidth; ++h) {
				for (int w = 0; w < qrCodeImageWidth; ++w) {
					int const offset = h * qrCodeImageWidth;

					if (qrCodeImageData[offset + w] != 0) {
						const int rx1 = w * scale, ry1 = h * scale;
						painter.fillRect(rx1, ry1, scale, scale, Qt::GlobalColor::black);
					}
				}
			}
		}
	}
}