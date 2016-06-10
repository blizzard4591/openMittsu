#include "widgets/ClickableLabel.h"

#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent) {
	//
}

ClickableLabel::~ClickableLabel() {
	//
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		emit clicked();
	}
}
