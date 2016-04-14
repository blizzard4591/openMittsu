#include "IdentityHelper.h"

#include "exceptions/NotConnectedException.h"
#include "exceptions/IllegalArgumentException.h"
#include "protocol/ProtocolSpecs.h"

#include <QRegExp>

bool IdentityHelper::isValidIdentityString(QString const& identity) {
	static QRegExp regExp("^[a-zA-Z0-9]{8}$", Qt::CaseInsensitive, QRegExp::RegExp2);

	return regExp.exactMatch(identity);
}

quint64 IdentityHelper::identityStringToUint64(QString const& identity) {
	QByteArray idBytes(identity.toLatin1());
	if (idBytes.size() != PROTO_IDENTITY_LENGTH_BYTES) {
		throw IllegalArgumentException() << "Invalid size of identity string.";
	}
	return *reinterpret_cast<quint64 const*>(idBytes.data());
}

QString IdentityHelper::uint64ToIdentityString(quint64 identity) {
	QByteArray idBytes(reinterpret_cast<char const*>(&identity), PROTO_IDENTITY_LENGTH_BYTES);
	for (int i = 0; i < PROTO_IDENTITY_LENGTH_BYTES; ++i) {
		if (!((('A' <= idBytes.at(i)) && (idBytes.at(i) <= 'Z')) || (('0' <= idBytes.at(i)) && (idBytes.at(i) <= '9')))) {
			throw IllegalArgumentException() << "Invalid identity string.";
		}
	}
	QString result(idBytes);
	if (result.size() != PROTO_IDENTITY_LENGTH_BYTES) {
		throw IllegalArgumentException() << "Invalid identity string.";
	}
	return result;
}

quint64 IdentityHelper::groupIdByteArrayToUint64(QByteArray const& groupId) {
	if (groupId.size() != PROTO_GROUP_GROUPID_LENGTH_BYTES) {
		throw IllegalArgumentException() << "Invalid size of groupId array.";
	}
	return *reinterpret_cast<quint64 const*>(groupId.data());
}

QByteArray IdentityHelper::uint64ToGroupIdByteArray(quint64 groupId) {
	QByteArray idBytes(reinterpret_cast<char const*>(&groupId), PROTO_GROUP_GROUPID_LENGTH_BYTES);
	return idBytes;
}