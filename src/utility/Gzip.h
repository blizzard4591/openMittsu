#ifndef OPENMITTSU_UTILITY_GZIP_H_
#define OPENMITTSU_UTILITY_GZIP_H_

// This is taken and modified from https://stackoverflow.com/a/24949005
// posted by Morix Dev

#include <QByteArray>

#define GZIP_WINDOW_BITS 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

class Gzip {
public:
	static bool gzipCompress(QByteArray const& input, QByteArray& output, int level = -1);
	static bool gzipDecompress(QByteArray const& input, QByteArray& output);
};

#endif // OPENMITTSU_UTILITY_GZIP_H_