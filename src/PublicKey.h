#ifndef OPENMITTSU_PUBLICKEY_H_
#define OPENMITTSU_PUBLICKEY_H_

#include "protocol/ProtocolSpecs.h"
#include "Key.h"
#include <QString>
#include <QByteArray>
#include <QMetaType>

class PublicKey : public Key {
public:
	PublicKey();
	PublicKey(PublicKey const& other);
	virtual ~PublicKey();

	virtual QByteArray const& getPrivateKey() const override;
	virtual QByteArray const& getPublicKey() const override;
	virtual QByteArray const& getIdentity() const override;

	virtual bool hasPrivateKey() const override;
	virtual bool hasPublicKey() const override;
	virtual bool hasIdentity() const override;

	virtual QByteArray const& getFingerprint() const;
	virtual QByteArray const& getFullFingerprint() const;

	virtual QString toString() const override;

	static PublicKey fromDecodedServerResponse(QByteArray const& response);
	static PublicKey fromHexString(QString const& hexString);

	bool operator ==(PublicKey const& other) const;
protected:
	QByteArray publicKey;
	QByteArray halfFingerprint;
	QByteArray fullFingerprint;
	void calculateFingerprintFromPublicKey();
};

Q_DECLARE_METATYPE(PublicKey)

#endif // OPENMITTSU_PUBLICKEY_H_