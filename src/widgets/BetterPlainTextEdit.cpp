#include "BetterPlainTextEdit.h"

BetterPlainTextEdit::BetterPlainTextEdit(QWidget *parent) : QPlainTextEdit(parent) {
	//
}

void BetterPlainTextEdit::keyReleaseEvent(QKeyEvent* e) {
	QPlainTextEdit::keyReleaseEvent(e);
}

void BetterPlainTextEdit::keyPressEvent(QKeyEvent* e) {
	if (e->isAutoRepeat()) {
		QPlainTextEdit::keyPressEvent(e);
		return;
	}
	if ((!(e->modifiers() & Qt::KeyboardModifier::ShiftModifier)) && (e->key() == Qt::Key::Key_Return)) {
		e->accept();
		emit returnPressed();
	} else {
		QPlainTextEdit::keyPressEvent(e);
	}
}