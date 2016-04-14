#include "Pkcs7.h"

#include <cstdint>
#include "sodium.h"

QByteArray Pkcs7::decodePkcs7Sequence(QByteArray const& source) {
	char lastByte = source.at(source.size() - 1);
	int paddingCount = *reinterpret_cast<unsigned char*>(&lastByte);

	return source.left(source.size() - paddingCount);
}

QByteArray Pkcs7::encodePkcs7Sequence(QByteArray const& source) {
	unsigned char paddingCount = 0xFF;
	randombytes_buf(&paddingCount, 1);
	// Test for zero
	if (paddingCount == 0x00) {
		++paddingCount;
	}

	QByteArray padding(static_cast<int>(paddingCount), *reinterpret_cast<char*>(&paddingCount));
	return QByteArray(source).append(padding);
}