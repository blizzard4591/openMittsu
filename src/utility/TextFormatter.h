#ifndef OPENMITTSU_UTILITY_TEXTFORMATTER_H_
#define OPENMITTSU_UTILITY_TEXTFORMATTER_H_

#include <QString>

namespace openmittsu {
	namespace utility {

		class TextFormatter {
		public:
			static QString format(QString const& input);
		private:
			TextFormatter() {}
			virtual ~TextFormatter() {}
		};

	}
}

#endif // OPENMITTSU_UTILITY_TEXTFORMATTER_H_