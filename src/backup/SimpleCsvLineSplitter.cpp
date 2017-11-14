#include "src/backup/SimpleCsvLineSplitter.h"

#include "src/exceptions/InvalidInputException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InsufficientInputException.h"

namespace openmittsu {
	namespace backup {

		SimpleCsvLineSplitter::SimpleCsvLineSplitter(QStringList const& columns) : m_columns(columns) {
			//
		}

		SimpleCsvLineSplitter::~SimpleCsvLineSplitter() {
			//
		}

		int SimpleCsvLineSplitter::getNumberOfColumns() const {
			return m_columns.size();
		}

		QString const& SimpleCsvLineSplitter::getColumn(int columnIndex) const {
			return m_columns.at(columnIndex);
		}

		SimpleCsvLineSplitter SimpleCsvLineSplitter::split(int numberOfColumns, QString const& line) {
			if (numberOfColumns < 1) {
				throw openmittsu::exceptions::IllegalArgumentException() << "The number of columns must be one or larger.";
			}

			int const lineSize = line.size();
			int const lineSizeMinusOne = lineSize - 1;
			bool inColumn = false;
			bool readIntro = false;
			QStringList result;

			int position = 0;
			QString currentLine;

			while (position < lineSize) {
				bool const atEnd = position >= lineSizeMinusOne;

				if (inColumn) {
					if (line.at(position) == '"') {
						if ((atEnd) || (line.at(position + 1) != '"')) {
							inColumn = false;
						} else {
							// escaped "
							currentLine.append(QStringLiteral("\""));
							position += 2;
						}
					} else {
						int index = line.indexOf('"', position);
						if (index == -1) {
							throw openmittsu::exceptions::InsufficientInputException() << "Expected a closing \" but there is none. Input as hex: " << QString(line.toUtf8().toHex()).toStdString();
						}
						currentLine.append(line.mid(position, index - position));
						position = index;
					}
				} else {
					if (line.at(position) != '"') {
						throw openmittsu::exceptions::InvalidInputException() << "Expected a \" at position " << position << ", but found \"" << line.at(position).toLatin1() << "\" instead. Input as hex: " << QString(line.toUtf8().toHex()).toStdString();
					}

					if (readIntro) {
						result.append(currentLine);
						currentLine.clear();
						if ((!atEnd) && (line.at(position + 1) != ',')) {
							throw openmittsu::exceptions::InvalidInputException() << "Expected a \",\" at position " << (position + 1) << ", but found \"" << line.at(position + 1).toLatin1() << "\" instead. Input as hex: " << QString(line.toUtf8().toHex()).toStdString();
						}

						readIntro = false;
						position += 2;
					} else {
						currentLine.clear();
						readIntro = true;
						inColumn = true;
						++position;
					}
				}
			}

			if (readIntro || inColumn) {
				throw openmittsu::exceptions::InsufficientInputException() << "Unexpected end of line! Input as hex: " << QString(line.toUtf8().toHex()).toStdString();
			}

			return SimpleCsvLineSplitter(result);
		}
	}
}
