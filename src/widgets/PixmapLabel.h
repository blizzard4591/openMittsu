#ifndef OPENMITTSU_WIDGETS_PIXMAPLABEL_H_
#define OPENMITTSU_WIDGETS_PIXMAPLABEL_H_

#include "src/widgets/ClickAwareLabel.h"

#include <QPixmap>
#include <QResizeEvent>

namespace openmittsu {
	namespace widgets {

		class PixmapLabel : public ClickAwareLabel {
			Q_OBJECT
		public:
			enum class Mode {
				HeightForWidth, TouchFromInside, FixedSize
			};

			explicit PixmapLabel(QWidget* parent = nullptr);
			virtual int heightForWidth(int width) const;
			virtual QSize sizeHint() const;

			virtual void setScalingFactor(double factor);
			virtual void setMode(Mode newMode);
		public slots:
			void setPixmap(QPixmap const& p);
			void resizeEvent(QResizeEvent* event);
		private:
			QPixmap m_pixmap;
			Mode m_mode;
			double m_scalingFactor;
		};

	}
}

#endif // OPENMITTSU_WIDGETS_PIXMAPLABEL_H_