#include "src/widgets/ClickAwareLabel.h"

#include <QMouseEvent>

namespace openmittsu {
	namespace widgets {

		ClickAwareLabel::ClickAwareLabel(QWidget* parent) : QLabel(parent) {
			//
		}

		ClickAwareLabel::~ClickAwareLabel() {
			//
		}

		void ClickAwareLabel::mousePressEvent(QMouseEvent* event) {
			if (event->buttons() == Qt::LeftButton) {
				emit clicked();
			}

			QLabel::mousePressEvent(event);
		}

		void ClickAwareLabel::mouseDoubleClickEvent(QMouseEvent *event) {
			QLabel::mouseDoubleClickEvent(event);

			if (this->hasSelectedText()) {
				emit doubleClickSelectedText(this->selectedText());
			}
		}

	}
}
