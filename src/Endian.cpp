#include "Endian.h"

#include <QByteArray>
#include <QtEndian>

QString Endian::getEndiannessDescriptionString() {
#if (Q_BYTE_ORDER) == (Q_LITTLE_ENDIAN)
	return QStringLiteral("LITTLE_ENDIAN");
#elif (Q_BYTE_ORDER) == (Q_BIG_ENDIAN)
	return QStringLiteral("BIG_ENDIAN");
#else
	return QStringLiteral("MIXED_ENDIAN");
#endif
}

uint16_t Endian::uint16FromLittleEndianToHostEndian(uint16_t source) {
	uint16_t target;

	target = qFromLittleEndian(static_cast<quint16>(source));

	return target;
}

uint32_t Endian::uint32FromLittleEndianToHostEndian(uint32_t source) {
	uint32_t target;

	target = qFromLittleEndian(static_cast<quint32>(source));

	return target;
}

uint64_t Endian::uint64FromLittleEndianToHostEndian(uint64_t source) {
	uint64_t target;

	target = qFromLittleEndian(static_cast<quint64>(source));

	return target;
}

uint16_t Endian::uint16FromBigEndianToHostEndian(uint16_t source) {
	uint16_t target;

	target = qFromBigEndian(static_cast<quint16>(source));

	return target;
}

uint32_t Endian::uint32FromBigEndianToHostEndian(uint32_t source) {
	uint32_t target;

	target = qFromBigEndian(static_cast<quint32>(source));

	return target;
}

uint64_t Endian::uint64FromBigEndianToHostEndian(uint64_t source) {
	uint64_t target;

	target = qFromBigEndian(static_cast<quint64>(source));

	return target;
}

uint16_t Endian::uint16FromHostEndianToBigEndian(uint16_t source) {
	uint16_t target;

	target = qToBigEndian(static_cast<quint16>(source));

	return target;
}

uint32_t Endian::uint32FromHostEndianToBigEndian(uint32_t source) {
	uint32_t target;

	target = qToBigEndian(static_cast<quint32>(source));

	return target;
}

uint64_t Endian::uint64FromHostEndianToBigEndian(uint64_t source) {
	uint64_t target;

	target = qToBigEndian(static_cast<quint64>(source));

	return target;
}

uint16_t Endian::uint16FromHostEndianToLittleEndian(uint16_t source) {
	uint16_t target;

	target = qToLittleEndian(static_cast<quint16>(source));

	return target;
}

uint32_t Endian::uint32FromHostEndianToLittleEndian(uint32_t source) {
	uint32_t target;

	target = qToLittleEndian(static_cast<quint32>(source));

	return target;
}

uint64_t Endian::uint64FromHostEndianToLittleEndian(uint64_t source) {
	uint64_t target;

	target = qToLittleEndian(static_cast<quint64>(source));

	return target;
}

QByteArray Endian::uint32FromHostToLittleEndianByteArray(quint32 source) {
	quint32 sourceInLittleEndian = Endian::uint32FromHostEndianToLittleEndian(source);

	Quint32CharAccess accessUnion;
	accessUnion.number = sourceInLittleEndian;

	QByteArray target(accessUnion.chars, 4);

	return target;
}
