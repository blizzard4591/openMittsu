#ifndef OPENMITTSU_ENCODING_PKCS7_H_
#define OPENMITTSU_ENCODING_PKCS7_H_

#include <QString>
#include <QChar>
#include <QByteArray>

namespace openmittsu {
	namespace encoding {

		class Pkcs7 {
		public:
			static QByteArray decodePkcs7Sequence(QByteArray const& source);
			static QByteArray encodePkcs7Sequence(QByteArray const& source);
		private:
			Pkcs7() {}
			virtual ~Pkcs7() {}
		};

	}
}

#endif // OPENMITTSU_ENCODING_PKCS7_H_