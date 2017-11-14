#ifndef OPENMITTSU_UTILITY_BYTEARRAYCONVERSIONS_H_
#define OPENMITTSU_UTILITY_BYTEARRAYCONVERSIONS_H_

#include <cstdint>
#include <QtGlobal>
#include <QByteArray>

namespace openmittsu {
	namespace utility {

		class ByteArrayConversions {
		public:
			static quint64 convert8ByteQByteArrayToQuint64(QByteArray const& eightBytes);
			static quint32 convert4ByteQByteArrayToQuint32(QByteArray const& fourBytes);
			static quint16 convert2ByteQByteArrayToQuint16(QByteArray const& twoBytes);

			static QByteArray convertQuint64toQByteArray(quint64 data);
			static QByteArray convertQuint32toQByteArray(quint32 data);
			static QByteArray convertQuint16toQByteArray(quint16 data);
		private:
			ByteArrayConversions();
			virtual ~ByteArrayConversions();
		};

	}
}

#endif // OPENMITTSU_UTILITY_BYTEARRAYCONVERSIONS_H_
