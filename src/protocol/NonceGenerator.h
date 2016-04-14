#ifndef OPENMITTSU_PROTOCOL_NONCEGENERATOR_H_
#define OPENMITTSU_PROTOCOL_NONCEGENERATOR_H_

#include "protocol/Nonce.h"
#include <QByteArray>
#include <QMutex>

class NonceGenerator {
public:
	NonceGenerator();
	NonceGenerator(QByteArray const& noncePrefix);
	virtual ~NonceGenerator();

	NonceGenerator& operator=(NonceGenerator const& other);

	QByteArray const& getNoncePrefix() const;
	Nonce getNextNonce();

	static int getNonceLength();
	static int getNoncePrefixLength();
private:
	QByteArray prefix;
	quint64 counter;

	QMutex mutex;
};

#endif // OPENMITTSU_PROTOCOL_NONCEGENERATOR_H_
