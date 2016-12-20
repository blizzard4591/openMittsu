#ifndef OPENMITTSU_WIDGETS_PIXMAPLABEL_H_
#define OPENMITTSU_WIDGETS_PIXMAPLABEL_H_

#include "widgets/ClickAwareLabel.h"

#include <QPixmap>
#include <QResizeEvent>

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
	QPixmap _pixmap;
	Mode mode;
	double scalingFactor;
};

#endif // OPENMITTSU_WIDGETS_PIXMAPLABEL_H_