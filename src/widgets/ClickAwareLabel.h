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
	void doubleClickSelectedText(QString const& selectedText);
protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // OPENMITTSU_WIDGETS_CLICKAWARELABEL_H_