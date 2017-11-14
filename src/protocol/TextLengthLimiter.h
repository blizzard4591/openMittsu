#ifndef OPENMITTSU_PROTOCOL_TEXTLENGTHLIMITER_H_
#define OPENMITTSU_PROTOCOL_TEXTLENGTHLIMITER_H_

#include "src/protocol/ProtocolSpecs.h"
#include <QString>
#include <QStringList>

namespace openmittsu {
	namespace protocol {

		class TextLengthLimiter {
		public:
			static QStringList splitTextForSending(QString const& text);
			static int getMaximumTextLength();
		private:
			virtual ~TextLengthLimiter() {}
		};

	}
}

#endif // OPENMITTSU_PROTOCOL_TEXTLENGTHLIMITER_H_
