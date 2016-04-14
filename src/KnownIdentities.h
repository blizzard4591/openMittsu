#ifndef OPENMITTSU_KNOWNIDENTITIES_H_
#define OPENMITTSU_KNOWNIDENTITIES_H_

#include <QObject>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QMutex>

#include <cstdint>

#include "PublicKey.h"
#include "KeyPair.h"

class KnownIdentities : public QObject {
	Q_OBJECT
public:
	static KnownIdentities* getInstance();

	void fromFile(QString const& filename, bool dryRun = false);
	void toFile(QString const& filename) const;
	bool hasIdentity(QString const& identity) const;
	bool hasNickname(QString const& identity) const;
	PublicKey const& getPublicKey(QString const& identity) const;
	QString const& getNickname(QString const& identity) const;
	int getIdentityCount() const;

	void addIdentity(QString const& identity, PublicKey const& publicKey);
	void setNickname(QString const& identity, QString const& nickName);

	QHash<QString, PublicKey>::const_iterator identitiesConstBegin() const;
	QHash<QString, PublicKey>::const_iterator identitiesConstEnd() const;
signals:
	void identitiesChanged();
private:
	KnownIdentities() {	}

	KnownIdentities(const KnownIdentities &); // hide copy constructor
	KnownIdentities& operator=(const KnownIdentities &); // hide assign op
	// we leave just the declarations, so the compiler will warn us 
	// if we try to use those two functions by accident

	static KnownIdentities* instance;

	mutable QMutex accessMutex;
	QHash<QString, PublicKey> identityToPublicKeyHashMap;
	QHash<QString, QString> identityToNicknameHashMap;
};

#endif // OPENMITTSU_KNOWNIDENTITIES_H_