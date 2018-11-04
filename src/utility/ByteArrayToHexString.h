#ifndef OPENMITTSU_UTILITY_BYTEARRAYTOHEXSTRING_H_
#define OPENMITTSU_UTILITY_BYTEARRAYTOHEXSTRING_H_

#include <QByteArray>
#include <QString>

class ByteArrayToHexString {
public:
	static QString charToHexString(char c) {
		return QString::number(c, 16).rightJustified(2, '0');
	}

	static QString convert(QByteArray const& sourceBytes) {
		QString resultHex;
		for (int i = 0; i < sourceBytes.size(); ++i) {
			resultHex.append(charToHexString(sourceBytes.at(i)));
		}
		return resultHex;
	}
private:
	ByteArrayToHexString() {}
	virtual ~ByteArrayToHexString() {}
};

#endif // OPENMITTSU_UTILITY_BYTEARRAYTOHEXSTRING_H_