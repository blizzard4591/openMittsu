#include "src/utility/StringList.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"


namespace openmittsu {
	namespace utility {

		StringList::StringList(QString const& list) : m_list(split(list)) {
			//
		}
		
		StringList::StringList(QStringList const& list) : m_list(list) {
			//
		}

		StringList::~StringList() {
			//
		}

		int StringList::size() const {
			return m_list.size();
		}
		
		QString StringList::toString() const {
			 // TODO
			return "";
		}

		QStringList StringList::split(QString const& s) {
			int const lineSize = s.size();
			int const lineSizeMinusOne = lineSize - 1;
			if ((s.at(0) == '[') && (s.at(lineSizeMinusOne) == ']')) {
				return split(s.mid(1, lineSize - 2));
			}

			QStringList result;
			bool inQuoted = false;
			QString currentColumn;

			int position = 0;
			while (position < lineSize) {
				bool const atEnd = position >= lineSizeMinusOne;

				if ((!inQuoted) && (s.at(position) == ',')) {
					result.append(currentColumn);
					currentColumn.clear();
					++position;
				} else if (s.at(position) == '"') {
					inQuoted = !inQuoted;
					++position;
				} else if (s.at(position) == '\\') {
					if (!atEnd) {
						QChar const nextChar = s.at(position + 1);
						currentColumn.append(nextChar);
						position += 2;
					} else {
						// Mumble, this is an escape sequence where it does not escape anything
						++position;
					}
				} else {
					int const nextComma = s.indexOf(',', position);
					int const nextBackslash = s.indexOf('\\', position);
					int const nextQuotationMark = s.indexOf('"', position);

					int const nextStop = std::min(std::min(((nextComma > -1) ? nextComma : lineSize), ((nextBackslash > -1) ? nextBackslash : lineSize)), ((nextQuotationMark > -1) ? nextQuotationMark : lineSize));
					if (nextStop > position) {
						currentColumn.append(s.mid(position, nextStop - position));
					}
					position = nextStop;
				}
			}

			result.append(currentColumn);
			return result;
		}
	}
}
