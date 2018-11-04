#include "src/widgets/UpperCaseLineEdit.h"

#include "src/utility/Logging.h"
#include "src/utility/QObjectConnectionMacro.h"

UpperCaseLineEdit::UpperCaseLineEdit(QWidget *parent) : QLineEdit(parent) {
	OPENMITTSU_CONNECT(this, textEdited(QString const&), this, onTextChanged(QString const&));
}

void UpperCaseLineEdit::onTextChanged(QString const& text) {
	QString const upperText = text.toUpper();
	// Check if lowercase letters are present
	if (upperText != text) {
		// Save Cursor position to enable mid-line editing
		int const cursorPosition = this->cursorPosition();
		this->setText(upperText);
		this->setCursorPosition(cursorPosition);
	}
}
