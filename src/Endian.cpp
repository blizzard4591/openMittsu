#include "Endian.h"

#include <QByteArray>
#include "endianness_config.h"

QString Endian::getEndiannessDescriptionString() {
#if (ENDIANNESS_CONFIG_ENDIAN_TYPE) == (ENDIANNESS_VAL_LITTLE_ENDIAN)
	return QStringLiteral("LITTLE_ENDIAN");
#elif (ENDIANNESS_CONFIG_ENDIAN_TYPE) == (ENDIANNESS_VAL_BIG_ENDIAN)
	return QStringLiteral("BIG_ENDIAN");
#else
	return QStringLiteral("MIXED_ENDIAN");
#endif
}

uint16_t Endian::uint16FromLittleEndianToHostEndian(uint16_t source) {
	uint16_t target;

	target = letoh16(source);

	return target;
}

uint32_t Endian::uint32FromLittleEndianToHostEndian(uint32_t source) {
	uint32_t target;

	target = letoh32(source);

	return target;
}

uint64_t Endian::uint64FromLittleEndianToHostEndian(uint64_t source) {
	uint64_t target;

	target = letoh64(source);

	return target;
}

uint16_t Endian::uint16FromBigEndianToHostEndian(uint16_t source) {
	uint16_t target;

	target = betoh16(source);

	return target;
}

uint32_t Endian::uint32FromBigEndianToHostEndian(uint32_t source) {
	uint32_t target;

	target = betoh32(source);

	return target;
}

uint64_t Endian::uint64FromBigEndianToHostEndian(uint64_t source) {
	uint64_t target;

	target = betoh64(source);

	return target;
}

uint16_t Endian::uint16FromHostEndianToBigEndian(uint16_t source) {
	uint16_t target;

	target = htobe16(source);

	return target;
}

uint32_t Endian::uint32FromHostEndianToBigEndian(uint32_t source) {
	uint32_t target;

	target = htobe32(source);

	return target;
}

uint64_t Endian::uint64FromHostEndianToBigEndian(uint64_t source) {
	uint64_t target;

	target = htobe64(source);

	return target;
}

uint16_t Endian::uint16FromHostEndianToLittleEndian(uint16_t source) {
	uint16_t target;

	target = htole16(source);

	return target;
}

uint32_t Endian::uint32FromHostEndianToLittleEndian(uint32_t source) {
	uint32_t target;

	target = htole32(source);

	return target;
}

uint64_t Endian::uint64FromHostEndianToLittleEndian(uint64_t source) {
	uint64_t target;

	target = htole64(source);

	return target;
}

QByteArray Endian::uint32FromHostToLittleEndianByteArray(quint32 source) {
	quint32 sourceInLittleEndian = htole32(source);

	Quint32CharAccess accessUnion;
	accessUnion.number = sourceInLittleEndian;

	QByteArray target(accessUnion.chars, 4);

	return target;
}
