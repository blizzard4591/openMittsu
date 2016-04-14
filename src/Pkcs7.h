#ifndef OPENMITTSU_PKCS7_H_
#define OPENMITTSU_PKCS7_H_

#include <QString>
#include <QChar>
#include <QByteArray>

class Pkcs7 {
public:
	static QByteArray decodePkcs7Sequence(QByteArray const& source);
	static QByteArray encodePkcs7Sequence(QByteArray const& source);
private:
	Pkcs7() {}
	~Pkcs7() {}
};

#endif // OPENMITTSU_PKCS7_H_