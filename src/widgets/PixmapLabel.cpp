#include "PixmapLabel.h"

PixmapLabel::PixmapLabel(QWidget *parent) : QLabel(parent) {
	this->setMinimumSize(1, 1);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	sizePolicy.setHeightForWidth(true);

	this->setSizePolicy(sizePolicy);
}

void PixmapLabel::setPixmap(QPixmap const& p) {
	pixmap = p;
	QLabel::setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

int PixmapLabel::heightForWidth(int width) const {
	return ((qreal)pixmap.height()*width) / pixmap.width();
}

QSize PixmapLabel::sizeHint() const {
	int w = this->width();
	return QSize(w, heightForWidth(w));
}

void PixmapLabel::resizeEvent(QResizeEvent* event) {
	QLabel::setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}