#ifndef OPENMITTSU_ENCODING_BASE32_H_
#define OPENMITTSU_ENCODING_BASE32_H_

#include <QString>
#include <QChar>
#include <QByteArray>

namespace openmittsu {
	namespace encoding {

		class Base32 {
		public:
			static char convertCharTo5BitValue(QChar const& inputChar);
			static char convert5BitValueToChar(char inputChar);
			static bool isCharacterOfBase32Encoding(QChar const& inputChar);
			static QByteArray decodeBase32Sequence(QString const& source);
			static QString encodeBase32Sequence(QByteArray const& source, bool formatString = true);
		private:
			Base32() {}
			virtual ~Base32() {}
		};

	}
}

#endif // OPENMITTSU_ENCODING_BASE32_H_