#ifndef OPENMITTSU_WIDGETS_CLICKAWARELABEL_H_
#define OPENMITTSU_WIDGETS_CLICKAWARELABEL_H_

#include <QLabel>
#include <QObject>

class QMouseEvent;

class ClickAwareLabel : public QLabel {
	Q_OBJECT
public:
	explicit ClickAwareLabel(QWidget* parent = nullptr);
	virtual ~ClickAwareLabel();
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event);
};

#endif // OPENMITTSU_WIDGETS_CLICKAWARELABEL_H_