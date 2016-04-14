#ifndef OPENMITTSU_IDENTITYHELPER_H_
#define OPENMITTSU_IDENTITYHELPER_H_

#include <QObject>
#include <QString>
#include <QByteArray>
#include <cstdint>

class IdentityHelper {
public:
	static bool isValidIdentityString(QString const& identity);
	static quint64 identityStringToUint64(QString const& identity);
	static QString uint64ToIdentityString(quint64 identity);

	static quint64 groupIdByteArrayToUint64(QByteArray const& groupId);
	static QByteArray uint64ToGroupIdByteArray(quint64 groupId);
private:
	IdentityHelper() {}
	~IdentityHelper() {}
};

#endif // OPENMITTSU_IDENTITYHELPER_H_