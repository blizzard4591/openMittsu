#ifndef OPENMITTSU_WIDGETS_CLICKABLELABEL_H_
#define OPENMITTSU_WIDGETS_CLICKABLELABEL_H_

#include <QLabel>
#include <QObject>

class QMouseEvent;

class ClickableLabel : public QLabel {
	Q_OBJECT
public:
	explicit ClickableLabel(QWidget* parent = nullptr);
	virtual ~ClickableLabel();
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event);
};

#endif // OPENMITTSU_WIDGETS_CLICKABLELABEL_H_