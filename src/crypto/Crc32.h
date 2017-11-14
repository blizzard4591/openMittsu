#ifndef OPENMITTSU_CRYPTO_CRC32_H_
#define OPENMITTSU_CRYPTO_CRC32_H_

#include <cstdlib>			/* For size_t */
#include <cstdint>			/* For uint8_t, uint16_t, uint32_t */
#include <QByteArray>

namespace openmittsu {
	namespace crypto {

		class Crc32 {
		public:
			static uint32_t checksum(QByteArray const& data);
			static QString toString(uint32_t checksum);
		private:
			static uint32_t crc32buf(char const* buf, size_t len);
		};

	}
}

#endif // OPENMITTSU_CRYPTO_CRC32_H_
