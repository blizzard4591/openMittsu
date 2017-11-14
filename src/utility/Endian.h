#ifndef OPENMITTSU_UTILITY_ENDIAN_H_
#define OPENMITTSU_UTILITY_ENDIAN_H_

#include <cstdint>
#include <QtGlobal>
#include <QString>
#include <QByteArray>

namespace openmittsu {
	namespace utility {

		class Endian {
		public:
			static uint16_t uint16FromLittleEndianToHostEndian(uint16_t source);
			static uint32_t uint32FromLittleEndianToHostEndian(uint32_t source);
			static uint64_t uint64FromLittleEndianToHostEndian(uint64_t source);

			static uint16_t uint16FromBigEndianToHostEndian(uint16_t source);
			static uint32_t uint32FromBigEndianToHostEndian(uint32_t source);
			static uint64_t uint64FromBigEndianToHostEndian(uint64_t source);

			static uint16_t uint16FromHostEndianToBigEndian(uint16_t source);
			static uint32_t uint32FromHostEndianToBigEndian(uint32_t source);
			static uint64_t uint64FromHostEndianToBigEndian(uint64_t source);

			static uint16_t uint16FromHostEndianToLittleEndian(uint16_t source);
			static uint32_t uint32FromHostEndianToLittleEndian(uint32_t source);
			static uint64_t uint64FromHostEndianToLittleEndian(uint64_t source);

			static QByteArray uint32FromHostToLittleEndianByteArray(quint32 source);

			static QString getEndiannessDescriptionString();
		private:
			Endian() {}
			virtual ~Endian() {}

			union Quint32CharAccess {
				quint32 number;
				char chars[4];
			};
		};

	}
}

#endif // OPENMITTSU_UTILITY_ENDIAN_H_