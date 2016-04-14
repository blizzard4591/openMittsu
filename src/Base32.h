#ifndef OPENMITTSU_BASE32_H_
#define OPENMITTSU_BASE32_H_

#include <QString>
#include <QChar>
#include <QByteArray>

class Base32 {
public:
	static char convertCharTo5BitValue(QChar const& inputChar);
	static char convert5BitValueToChar(char inputChar);
	static bool isCharacterOfBase32Encoding(QChar const& inputChar);
	static QByteArray decodeBase32Sequence(QString const& source);
	static QString encodeBase32Sequence(QByteArray const& source, bool formatString = true);
private:
	Base32() {}
	~Base32() {}
};

#endif // OPENMITTSU_BASE32_H_