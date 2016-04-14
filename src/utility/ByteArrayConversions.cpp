#include "utility/ByteArrayConversions.h"

#include "exceptions/IllegalArgumentException.h"

ByteArrayConversions::ByteArrayConversions() {
	throw;
}

ByteArrayConversions::~ByteArrayConversions() {
	// Intentionally left empty.
}

quint64 ByteArrayConversions::convert8ByteQByteArrayToQuint64(QByteArray const& eightBytes) {
	if (eightBytes.size() != 8) {
		throw IllegalArgumentException() << "Invalid size of input byte-array: " << eightBytes.size() << " instead of 8 Bytes.";
	}

	return *reinterpret_cast<quint64 const*>(eightBytes.data());
}

quint32 ByteArrayConversions::convert4ByteQByteArrayToQuint32(QByteArray const& fourBytes) {
	if (fourBytes.size() != 4) {
		throw IllegalArgumentException() << "Invalid size of input byte-array: " << fourBytes.size() << " instead of 4 Bytes.";
	}

	return *reinterpret_cast<quint32 const*>(fourBytes.data());
}

quint16 ByteArrayConversions::convert2ByteQByteArrayToQuint16(QByteArray const& twoBytes) {
	if (twoBytes.size() != 2) {
		throw IllegalArgumentException() << "Invalid size of input byte-array: " << twoBytes.size() << " instead of 2 Bytes.";
	}

	return *reinterpret_cast<quint16 const*>(twoBytes.data());
}

QByteArray ByteArrayConversions::convertQuint64toQByteArray(quint64 data) {
	QByteArray result(8, 0x00);
	quint64* ptr = reinterpret_cast<quint64*>(result.data());
	*ptr = data;

	return result;
}

QByteArray ByteArrayConversions::convertQuint32toQByteArray(quint32 data) {
	QByteArray result(4, 0x00);
	quint32* ptr = reinterpret_cast<quint32*>(result.data());
	*ptr = data;

	return result;
}

QByteArray ByteArrayConversions::convertQuint16toQByteArray(quint16 data) {
	QByteArray result(2, 0x00);
	quint16* ptr = reinterpret_cast<quint16*>(result.data());
	*ptr = data;

	return result;
}
