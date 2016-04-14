#include "KnownIdentities.h"

#include "exceptions/IllegalArgumentException.h"
#include <QFile>
#include <QTextStream>
#include <QRegExp>

// Static Initializers
KnownIdentities* KnownIdentities::instance = nullptr;


KnownIdentities* KnownIdentities::getInstance() {
	static QMutex mutex;
	if (instance == nullptr) {
		mutex.lock();

		if (instance == nullptr) {
			instance = new KnownIdentities();
		}

		mutex.unlock();
	}

	return instance;
}

void KnownIdentities::fromFile(QString const& filename, bool dryRun) {
	if (!QFile::exists(filename)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file as it does not exist: %1").arg(filename).toStdString();
	}
	QFile inputFile(filename);
	if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file for reading: %1").arg(filename).toStdString();
	}

	QRegExp commentRegExp("^\\s*#.*$", Qt::CaseInsensitive, QRegExp::RegExp2);
	QRegExp identityRegExp("^\\s*([A-Z0-9]{8})\\s*:\\s*([a-fA-F0-9]{64})\\s*(?::\\s*(.*)\\s*)?$", Qt::CaseInsensitive, QRegExp::RegExp2);

	QTextStream in(&inputFile);
	while (!in.atEnd()) {
		QString line = in.readLine();
		if (line.trimmed().isEmpty() || commentRegExp.exactMatch(line)) {
			continue;
		} else if (identityRegExp.exactMatch(line)) {
			if (!dryRun) {
				accessMutex.lock();

				identityToPublicKeyHashMap.insert(identityRegExp.cap(1), PublicKey::fromHexString(identityRegExp.cap(2)));
				if (!identityRegExp.cap(3).trimmed().isEmpty()) {
					// Nickname given.
					identityToNicknameHashMap.insert(identityRegExp.cap(1), identityRegExp.cap(3));
				}

				accessMutex.unlock();
			}
		} else {
			throw IllegalArgumentException() << QString("Invalid or ill-formated line in contacts file \"%1\". Problematic line: %2").arg(filename).arg(line).toStdString();
		}
	}

	inputFile.close();
	emit identitiesChanged();
}

int KnownIdentities::getIdentityCount() const {
	return identityToPublicKeyHashMap.size();
}

void KnownIdentities::toFile(QString const& filename) const {
	QFile outputFile(filename);
	if (!outputFile.open(QFile::WriteOnly | QFile::Text)) {
		throw IllegalArgumentException() << QString("Could not open the specified contacts file for writing: %1").arg(filename).toStdString();
	}

	QTextStream outStream(&outputFile);
	outStream << "# This is a comment line.\n";
	outStream << "# Format of this file: \n";
	outStream << "# IDENTITY : PUBKEY : Nickname\n";
	outStream << "# where \n";
	outStream << "# - IDENTITY is an eight character ID of the form [A-Z0-9]{8} and stands for a users public id,\n";
	outStream << "# - PUBKEY is an 64 character key of the form [a-fA-F0-9]{64} and stands for a users 32-Byte long-term public key,\n";
	outStream << "# - Nickname is an optional screen-name for the given identity.\n";

	accessMutex.lock();
	QHash<QString, PublicKey>::const_iterator i;
	for (i = identityToPublicKeyHashMap.constBegin(); i != identityToPublicKeyHashMap.constEnd(); ++i) {
		if (identityToNicknameHashMap.contains(i.key())) {
			QString const& nickname = identityToNicknameHashMap.constFind(i.key()).value();
			outStream << i.key() << " : " << QString(i.value().getPublicKey().toHex()) << " : " << nickname << "\n";
		} else {
			outStream << i.key() << " : " << QString(i.value().getPublicKey().toHex()) << "\n";
		}
	}
	accessMutex.unlock();

	outputFile.close();
}

bool KnownIdentities::hasIdentity(QString const& identity) const {
	accessMutex.lock();
	bool result = identityToPublicKeyHashMap.contains(identity);
	accessMutex.unlock();

	return result;
}

bool KnownIdentities::hasNickname(QString const& identity) const {
	accessMutex.lock();
	bool result = identityToNicknameHashMap.contains(identity);
	accessMutex.unlock();

	return result;
}

PublicKey const& KnownIdentities::getPublicKey(QString const& identity) const {
	accessMutex.lock();
	PublicKey const& result = identityToPublicKeyHashMap.find(identity).value();
	accessMutex.unlock();

	return result;
}

QString const& KnownIdentities::getNickname(QString const& identity) const {
	accessMutex.lock();
	QString const& result = identityToNicknameHashMap.find(identity).value();
	accessMutex.unlock();

	return result;
}

void KnownIdentities::addIdentity(QString const& identity, PublicKey const& publicKey) {
	accessMutex.lock();
	identityToPublicKeyHashMap.insert(identity, publicKey);
	accessMutex.unlock();
	emit identitiesChanged();
}

void KnownIdentities::setNickname(QString const& identity, QString const& nickName) {
	accessMutex.lock();
	identityToNicknameHashMap.insert(identity, nickName);
	accessMutex.unlock();
	emit identitiesChanged();
}

QHash<QString, PublicKey>::const_iterator KnownIdentities::identitiesConstBegin() const {
	return identityToPublicKeyHashMap.constBegin();
}

QHash<QString, PublicKey>::const_iterator KnownIdentities::identitiesConstEnd() const {
	return identityToPublicKeyHashMap.constEnd();
}