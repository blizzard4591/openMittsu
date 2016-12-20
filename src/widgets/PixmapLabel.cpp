#include "PixmapLabel.h"

#include "utility/Logging.h"

PixmapLabel::PixmapLabel(QWidget *parent) : ClickAwareLabel(parent), mode(Mode::HeightForWidth), scalingFactor(1.0) {
	this->setMinimumSize(1, 1);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	sizePolicy.setHeightForWidth(true);

	this->setSizePolicy(sizePolicy);
}

void PixmapLabel::setPixmap(QPixmap const& p) {
	_pixmap = p;
	if (mode == Mode::FixedSize) {
		QSize size = _pixmap.size() * scalingFactor;
		this->resize(size);
		QLabel::setPixmap(_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		LOGGER_DEBUG("Setting pixmap in FixedSize mode with Size {} x {} (Pixmap size = {} x {}, Scaling Factor = {})", size.width(), size.height(), _pixmap.width(), _pixmap.height(), scalingFactor);
	} else if (mode == Mode::HeightForWidth) {
		QSize size = this->size();
		QLabel::setPixmap(_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		LOGGER_DEBUG("Setting pixmap in HeightForWidth mode with Size {} x {} (Pixmap size = {} x {})", size.width(), size.height(), _pixmap.width(), _pixmap.height());
	} else if (mode == Mode::TouchFromInside) {
		QSize size = this->size();
		QLabel::setPixmap(_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		LOGGER_DEBUG("Setting pixmap in TouchFromInside mode with Size {} x {} (Pixmap size = {} x {})", size.width(), size.height(), _pixmap.width(), _pixmap.height());
	}
}

int PixmapLabel::heightForWidth(int width) const {
	LOGGER_DEBUG("heightForWidth called with width = {}.", width);
	return ((qreal)_pixmap.height()*width) / _pixmap.width();
}

QSize PixmapLabel::sizeHint() const {
	if (mode == Mode::FixedSize) {
		QSize size = _pixmap.size() * scalingFactor;
		return size;
	} else if (mode == Mode::HeightForWidth) {
		int const w = this->width();
		int const h = this->height();
		int const hfW = ((qreal)_pixmap.height()*w) / _pixmap.width();

		return QSize(w, hfW);
	} else if (mode == Mode::TouchFromInside) {
		int const w = this->width();
		int const h = this->height();
		int const hfW = ((qreal)_pixmap.height()*w) / _pixmap.width();
		int const wfH = ((qreal)_pixmap.width()*h) / _pixmap.height();

		LOGGER_DEBUG("SizeHint in TouchFromInside mode: w = {}, h = {}, hfW = {}, wfH = {}.", w, h, hfW, wfH);
		if (hfW > h) {
			return QSize(wfH, h);
		} else {
			return QSize(w, hfW);
		}
	} else {
		throw;
	}
}

void PixmapLabel::resizeEvent(QResizeEvent* event) {
	QSize targetSize = sizeHint();
	LOGGER_DEBUG("Resize event occured, resizing to {} x {} with container size {} x {}.", targetSize.width(), targetSize.height(), this->width(), this->height());
	QLabel::setPixmap(_pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PixmapLabel::setScalingFactor(double factor) {
	if (mode == Mode::FixedSize) {
		if (factor > 0.05 && factor <= 5.0) {
			LOGGER_DEBUG("Scaling factor set to {}, resizing now.", factor);
			scalingFactor = factor;

			this->resize(_pixmap.size() * scalingFactor);
		}
	}
}

void PixmapLabel::setMode(Mode newMode) {
	if (newMode == Mode::FixedSize) {
		scalingFactor = 1.0;
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHeightForWidth(false);
		this->setSizePolicy(sizePolicy);
		LOGGER_DEBUG("Switching to mode FixedSize.");
	} else if (newMode == Mode::HeightForWidth) {
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		sizePolicy.setHeightForWidth(true);
		this->setSizePolicy(sizePolicy);
		LOGGER_DEBUG("Switching to mode HeightForWidth.");
	} else if (newMode == Mode::TouchFromInside) {
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHeightForWidth(false);
		this->setSizePolicy(sizePolicy);
		LOGGER_DEBUG("Switching to mode TouchFromInside.");
	}
	
	mode = newMode;
	QPixmap const tempPixmap = _pixmap;
	QLabel::setPixmap(tempPixmap);
}
