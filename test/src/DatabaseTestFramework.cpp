#include "DatabaseTestFramework.h"

QString DatabaseTestFramework::databaseFilename = QStringLiteral("testFile.sqlite");
openmittsu::protocol::ContactId DatabaseTestFramework::selfContactId(QStringLiteral("AAAAAAAA"));
openmittsu::crypto::KeyPair DatabaseTestFramework::selfKeyPair(openmittsu::crypto::KeyPair::randomKey());
