#ifndef OPENMITTSU_WIDGETS_BETTERPLAINTEXTEDIT_H_
#define OPENMITTSU_WIDGETS_BETTERPLAINTEXTEDIT_H_

#include <QPlainTextEdit>
#include <QKeyEvent>

class BetterPlainTextEdit : public QPlainTextEdit {
	Q_OBJECT
public:
	explicit BetterPlainTextEdit(QWidget* parent = nullptr);
protected:
	virtual void keyReleaseEvent(QKeyEvent *e) override;
	virtual void keyPressEvent(QKeyEvent *e) override;
signals:
	void returnPressed();
};

#endif // OPENMITTSU_WIDGETS_BETTERPLAINTEXTEDIT_H_