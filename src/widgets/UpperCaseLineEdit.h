#ifndef OPENMITTSU_WIDGETS_UPPERCASELINEEDIT_H_
#define OPENMITTSU_WIDGETS_UPPERCASELINEEDIT_H_

#include <QLineEdit>
#include <QKeyEvent>

class UpperCaseLineEdit : public QLineEdit {
	Q_OBJECT
public:
	explicit UpperCaseLineEdit(QWidget* parent = nullptr);
private slots:
	virtual void onTextChanged(QString const& text);
};

#endif // OPENMITTSU_WIDGETS_UPPERCASELINEEDIT_H_