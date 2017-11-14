#include "PixmapLabel.h"

#include "src/utility/Logging.h"

namespace openmittsu {
	namespace widgets {

		PixmapLabel::PixmapLabel(QWidget *parent) : ClickAwareLabel(parent), m_mode(Mode::HeightForWidth), m_scalingFactor(1.0) {
			this->setMinimumSize(1, 1);

			QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			sizePolicy.setHeightForWidth(true);

			this->setSizePolicy(sizePolicy);
		}

		void PixmapLabel::setPixmap(QPixmap const& p) {
			m_pixmap = p;
			if (m_mode == Mode::FixedSize) {
				QSize size = m_pixmap.size() * m_scalingFactor;
				this->resize(size);
				QLabel::setPixmap(m_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
				LOGGER_DEBUG("Setting pixmap in FixedSize mode with Size {} x {} (Pixmap size = {} x {}, Scaling Factor = {})", size.width(), size.height(), m_pixmap.width(), m_pixmap.height(), m_scalingFactor);
			} else if (m_mode == Mode::HeightForWidth) {
				QSize size = this->size();
				QLabel::setPixmap(m_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
				LOGGER_DEBUG("Setting pixmap in HeightForWidth mode with Size {} x {} (Pixmap size = {} x {})", size.width(), size.height(), m_pixmap.width(), m_pixmap.height());
			} else if (m_mode == Mode::TouchFromInside) {
				QSize size = this->size();
				QLabel::setPixmap(m_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
				LOGGER_DEBUG("Setting pixmap in TouchFromInside mode with Size {} x {} (Pixmap size = {} x {})", size.width(), size.height(), m_pixmap.width(), m_pixmap.height());
			}
		}

		int PixmapLabel::heightForWidth(int width) const {
			LOGGER_DEBUG("heightForWidth called with width = {}.", width);
			return ((qreal)m_pixmap.height()*width) / m_pixmap.width();
		}

		QSize PixmapLabel::sizeHint() const {
			if (m_mode == Mode::FixedSize) {
				QSize size = m_pixmap.size() * m_scalingFactor;
				return size;
			} else if (m_mode == Mode::HeightForWidth) {
				int const w = this->width();
				int const hfW = ((qreal)m_pixmap.height()*w) / m_pixmap.width();

				return QSize(w, hfW);
			} else if (m_mode == Mode::TouchFromInside) {
				int const w = this->width();
				int const h = this->height();
				int const hfW = ((qreal)m_pixmap.height()*w) / m_pixmap.width();
				int const wfH = ((qreal)m_pixmap.width()*h) / m_pixmap.height();

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
			QLabel::setPixmap(m_pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}

		void PixmapLabel::setScalingFactor(double factor) {
			if (m_mode == Mode::FixedSize) {
				if (factor > 0.05 && factor <= 5.0) {
					LOGGER_DEBUG("Scaling factor set to {}, resizing now.", factor);
					m_scalingFactor = factor;

					this->resize(m_pixmap.size() * m_scalingFactor);
				}
			}
		}

		void PixmapLabel::setMode(Mode newMode) {
			if (newMode == Mode::FixedSize) {
				m_scalingFactor = 1.0;
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

			m_mode = newMode;
			QPixmap const tempPixmap = m_pixmap;
			QLabel::setPixmap(tempPixmap);
		}

	}
}
