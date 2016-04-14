#ifndef OPENMITTSU_KEYPAIR_H_
#define OPENMITTSU_KEYPAIR_H_

#include "protocol/ProtocolSpecs.h"
#include "PublicKey.h"
#include <QString>
#include <QByteArray>

class KeyPair : public PublicKey {
public:
	KeyPair();
	KeyPair(KeyPair const& other);
	virtual ~KeyPair();

	virtual QByteArray const& getPrivateKey() const override;
	virtual QByteArray const& getIdentity() const override;

	virtual bool hasPrivateKey() const override;

	virtual QString toString() const override;

	static KeyPair fromBackup(QString const& base32Backup, QString const& password);
	static KeyPair randomKey();
	static KeyPair fromArrays(unsigned char const* pubKey, unsigned char const* secKey);
protected:
	QByteArray privateKey;
};

#endif // OPENMITTSU_KEYPAIR_H_