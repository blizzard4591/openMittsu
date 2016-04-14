#ifndef OPENMITTSU_KEY_H_
#define OPENMITTSU_KEY_H_

#include "protocol/ProtocolSpecs.h"
#include "sodium.h"

class QString;
class QByteArray;

class Key {
public:
	Key() {}
	virtual ~Key() {}

	virtual QByteArray const& getPrivateKey() const = 0;
	virtual QByteArray const& getPublicKey() const = 0;
	virtual QByteArray const& getIdentity() const = 0;

	virtual bool hasPrivateKey() const = 0;
	virtual bool hasPublicKey() const = 0;
	virtual bool hasIdentity() const = 0;

	virtual QString toString() const = 0;

	static int getPrivateKeyLength() {
		return PROTO_KEY_LENGTH_BYTES;
	}

	static int getPublicKeyLength() {
		return PROTO_KEY_LENGTH_BYTES;
	}

	static int getIndentityLength() {
		return BACKUP_IDENTITY_BYTES;
	}

};

#endif // OPENMITTSU_KEY_H_