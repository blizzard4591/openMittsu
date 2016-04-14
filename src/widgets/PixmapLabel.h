#ifndef OPENMITTSU_WIDGETS_PIXMAPLABEL_H_
#define OPENMITTSU_WIDGETS_PIXMAPLABEL_H_

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class PixmapLabel : public QLabel {
	Q_OBJECT
public:
	explicit PixmapLabel(QWidget* parent = nullptr);
	virtual int heightForWidth(int width) const;
	virtual QSize sizeHint() const;
public slots:
	void setPixmap(QPixmap const& p);
	void resizeEvent(QResizeEvent* event);
private:
	QPixmap pixmap;
};

#endif // OPENMITTSU_WIDGETS_PIXMAPLABEL_H_