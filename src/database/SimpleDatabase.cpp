#include "src/database/SimpleDatabase.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <QUuid>
#include <QSet>

#include <iostream>
#include "src/database/internal/DatabaseContactMessageCursor.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/exceptions/InvalidPasswordOrDatabaseException.h"
#include "src/exceptions/MissingQSqlCipherException.h"
#include "src/exceptions/UuidAlreadyExistsException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"

#include "Config.h"

namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

		SimpleDatabase::SimpleDatabase(QString const& filename, QString const& password, QDir const& mediaStorageLocation, bool useCompatibilityToVersionThree) : Database(), database(), m_driverNameCrypto("QSQLCIPHER"), m_driverNameStandard("QSQLITE"), m_connectionName(QStringLiteral("openMittsuDatabaseConnection%1").arg(QDateTime::currentMSecsSinceEpoch())), m_password(password), m_selfContact(0), m_selfLongTermKeyPair(), m_identityBackup(), m_contactAndGroupDataProvider(this, this), m_mediaFileStorage(mediaStorageLocation, this) {
			if (!(QSqlDatabase::isDriverAvailable(m_driverNameCrypto) || QSqlDatabase::isDriverAvailable(m_driverNameStandard))) {
				throw openmittsu::exceptions::InternalErrorException() << "Neither the SQL driver " << m_driverNameCrypto.toStdString() << " nor the driver " << m_driverNameStandard.toStdString() << " are available. Available are: " << QSqlDatabase::drivers().join(", ").toStdString();
			}

			if (QSqlDatabase::isDriverAvailable(m_driverNameCrypto)) {
				LOGGER()->info("Using the crypto-database interface (QSQLCIPHER).");
				database = QSqlDatabase::addDatabase(m_driverNameCrypto, m_connectionName);
				m_usingCryptoDb = true;
			} else {
#ifdef OPENMITTSU_CONFIG_ALLOW_MISSING_QSQLCIPHER
				LOGGER()->info("Using the non-crypto-database interface (QSQLITE).");
				database = QSqlDatabase::addDatabase(m_driverNameStandard, m_connectionName);
				m_usingCryptoDb = false;
#else
				LOGGER()->error("QSqlCipher is not available, no encryption available. Quiting!");
				throw openmittsu::exceptions::MissingQSqlCipherException() << "QSqlCipher is not available, no encryption available!";
#endif
			}
			database.setDatabaseName(filename);
			if (!database.open()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file.";
			} else if (database.isOpenError()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, error: " << database.lastError().text().toStdString();
			} else if (!database.isOpen()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, open failed.";
			}

			if (m_usingCryptoDb) {
				QString const sqlCipherVersion = getSqlCipherVersion();
				LOGGER()->info("SQLCipher Version reported as '{}'", sqlCipherVersion.toStdString());
			}

			setKey(m_password);
			if (m_usingCryptoDb && useCompatibilityToVersionThree) {
				QString const sqlCipherVersion = getSqlCipherVersion();
				if (!sqlCipherVersion.startsWith('3')) {
					setCompatibilityMode(true);
					LOGGER_DEBUG("Trying to open database file with backwards compatiblity flags for version 3!");
				} else {
					LOGGER()->info("SQLCipher Version 3 detected, ignoring request for opening database with compatiblity flags.");
				}
			}

			if (!isDatabaseFileReadableAndValid()) {
				throw openmittsu::exceptions::InvalidPasswordOrDatabaseException() << "SQLITE master table does not exist or not readable, invalid database or incorrect password.";
			}

			createOrUpdateTables();

			updateCachedIdentityBackup();
			m_selfContact = m_identityBackup->getClientContactId();
			m_selfLongTermKeyPair = m_identityBackup->getClientLongTermKeyPair();

			if (!hasContact(m_selfContact)) {
				storeNewContact(m_selfContact, m_selfLongTermKeyPair);
			}

			setupQueueTimer();
		}

		SimpleDatabase::SimpleDatabase(QString const& filename, openmittsu::protocol::ContactId const& selfContact, openmittsu::crypto::KeyPair const& selfLongTermKeyPair, QString const& password, QDir const& mediaStorageLocation) : Database(), database(), m_driverNameCrypto("QSQLCIPHER"), m_driverNameStandard("QSQLITE"), m_connectionName("openMittsuDatabaseConnection"), m_password(password), m_selfContact(selfContact), m_selfLongTermKeyPair(selfLongTermKeyPair), m_identityBackup(std::make_unique<openmittsu::backup::IdentityBackup>(selfContact, selfLongTermKeyPair)), m_contactAndGroupDataProvider(this, this), m_mediaFileStorage(mediaStorageLocation, this) {
			if (!(QSqlDatabase::isDriverAvailable(m_driverNameCrypto) || QSqlDatabase::isDriverAvailable(m_driverNameStandard))) {
				throw openmittsu::exceptions::InternalErrorException() << "Neither the SQL driver " << m_driverNameCrypto.toStdString() << " nor the driver " << m_driverNameStandard.toStdString() << " are available. Available are: " << QSqlDatabase::drivers().join(", ").toStdString();
			}

			if (QSqlDatabase::isDriverAvailable(m_driverNameCrypto)) {
				LOGGER()->info("Using the crypto-database interface (QSQLCIPHER).");
				database = QSqlDatabase::addDatabase(m_driverNameCrypto, m_connectionName);
				m_usingCryptoDb = true;
			} else {
#ifdef OPENMITTSU_CONFIG_ALLOW_MISSING_QSQLCIPHER
				LOGGER()->info("Using the non-crypto-database interface (QSQLITE).");
				database = QSqlDatabase::addDatabase(m_driverNameStandard, m_connectionName);
				m_usingCryptoDb = false;
#else
				LOGGER()->error("QSqlCipher is not available, no encryption available. Quiting!");
				throw openmittsu::exceptions::MissingQSqlCipherException() << "QSqlCipher is not available, no encryption available!";
#endif
			}
			database.setDatabaseName(filename);
			if (!database.open()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file.";
			} else if (database.isOpenError()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, error: " << database.lastError().text().toStdString();
			} else if (!database.isOpen()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, open failed.";
			}

			bool useCompatMode = false;
			if (m_usingCryptoDb) {
				QString const sqlCipherVersion = getSqlCipherVersion();
				LOGGER()->info("SQLCipher Version reported as '{}'", sqlCipherVersion.toStdString());
				if (!sqlCipherVersion.startsWith('3')) {
					useCompatMode = true;
				}
			}

			setKey(m_password);
			if (useCompatMode) {
				setCompatibilityMode(true);
			}

			QStringList const tables = database.tables(QSql::AllTables);
			if (!tables.contains(QStringLiteral("sqlite_master"))) {
				throw openmittsu::exceptions::InternalErrorException() << "SQLITE master table does not exist, invalid database.";
			}

			createOrUpdateTables();

			setBackup(selfContact, selfLongTermKeyPair);
			if (!hasContact(selfContact)) {
				storeNewContact(selfContact, selfLongTermKeyPair);
			} else {
				openmittsu::crypto::PublicKey const oldSelfPubKey = getContactPublicKey(selfContact);
				openmittsu::crypto::PublicKey const newSelfPubKey = selfLongTermKeyPair;
				if (!(oldSelfPubKey == newSelfPubKey)) {
					LOGGER()->error("Replacing old public key \"{}\" registered with ID {} with new public key \"{}\". This should never occur?!", oldSelfPubKey.toQString().toStdString(), selfContact.toString(), newSelfPubKey.toQString().toStdString());
					// TODO
				}
			}
			setupQueueTimer();
		}

		SimpleDatabase::~SimpleDatabase() {
			if (database.isOpen()) {
				database.close();
				database.removeDatabase(m_connectionName);
			}
		}

		QString SimpleDatabase::getSqlCipherVersion() {
			if (m_usingCryptoDb) {
				QSqlQuery query(database);
				if (!query.exec(QStringLiteral("PRAGMA cipher_version"))) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not query SQLCipher! Error was: " << query.lastError().text().toStdString();
				} else {
					if (query.isSelect()) {
						query.next();
						return query.value(QStringLiteral("cipher_version")).toString();
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Could not query SQLCipher version! Response was not selectable?!";
					}
				}
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "SQLCipher version string requested, but we are not using the crypto interface!";
			}
		}

		bool SimpleDatabase::isDatabaseFileReadableAndValid() {
			QStringList const tables = database.tables(QSql::AllTables);
			if (!tables.contains(QStringLiteral("sqlite_master"))) {
				return false;
			} else {
				QSqlQuery query(database);
				if (!query.exec(QStringLiteral("SELECT `type`, `name`, `sql`, `tbl_name` FROM `sqlite_master`"))) {
					return false;
				}
			}
			return true;
		}

		void SimpleDatabase::setCompatibilityMode(bool activate) {
			if (m_usingCryptoDb) {
				QSqlQuery query(database);

				if (activate) {
					if (!query.exec(QStringLiteral("PRAGMA cipher_page_size = 1024;"))) {
						LOGGER()->error("Could not set SqlCipher PRAGMA 1 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA kdf_iter = 64000;"))) {
						LOGGER()->error("Could not set SqlCipher PRAGMA 2 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA cipher_hmac_algorithm = HMAC_SHA1;"))) {
						LOGGER()->error("Could not set SqlCipher PRAGMA 3 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA cipher_kdf_algorithm = PBKDF2_HMAC_SHA1;"))) {
						LOGGER()->error("Could not set SqlCipher PRAGMA 4 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
				} else {
					if (!query.exec(QStringLiteral("PRAGMA cipher_page_size = 4096;"))) {
						LOGGER()->error("Could not unset SqlCipher PRAGMA 1 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA kdf_iter = 256000;"))) {
						LOGGER()->error("Could not unset SqlCipher PRAGMA 2 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA cipher_hmac_algorithm = HMAC_SHA512;"))) {
						LOGGER()->error("Could not unset SqlCipher PRAGMA 3 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
					if (!query.exec(QStringLiteral("PRAGMA cipher_kdf_algorithm = PBKDF2_HMAC_SHA512;"))) {
						LOGGER()->error("Could not unset SqlCipher PRAGMA 4 for compatibility with old format! Error was: {}", query.lastError().text().toStdString());
					}
				}
			}
		}

		void SimpleDatabase::setKey(QString const& password) {
			if (m_usingCryptoDb) {
				QSqlQuery query(database);
				bool needsEncoding = false;
				for (int i = 0; i < password.size(); ++i) {
					ushort const unicodeCodepoint = password.at(i).unicode();
					// Non-printable, above 127 or the ' or \ character
					if (((unicodeCodepoint < 32) || (unicodeCodepoint > 126)) || (unicodeCodepoint == 39) || (unicodeCodepoint == 92)) {
						needsEncoding = true;
						break;
					}
				}

				if (needsEncoding) {
					LOGGER()->info("The password contains non-printable or non-basic-ASCII characters. Converting to UTF-8 and stringifying resulting hexadecimal sequence.");
					query.exec(QStringLiteral("PRAGMA key = '%1';").arg(QString(password.toUtf8().toHex())));
				} else {
					query.exec(QStringLiteral("PRAGMA key = '%1';").arg(password));
				}

				query.finish();
			}
		}

		void SimpleDatabase::enableTimers() {
			queueTimeoutTimer.start();

			QTimer::singleShot(500, this, SLOT(onQueueTimeoutTimerFire()));
		}

		void SimpleDatabase::setupQueueTimer() {
			OPENMITTSU_CONNECT_QUEUED(&queueTimeoutTimer, timeout(), this, onQueueTimeoutTimerFire());
			queueTimeoutTimer.setInterval(30 * 1000);
		}

		void SimpleDatabase::onQueueTimeoutTimerFire() {
			LOGGER_DEBUG("Database queue timeout timer fired, checking database...");

			bool resetQueueChangedAnItem = false;
			resetQueueChangedAnItem = resetQueueChangedAnItem || internal::DatabaseContactMessage::resetQueueStatus(this, 30);
			resetQueueChangedAnItem = resetQueueChangedAnItem || internal::DatabaseGroupMessage::resetQueueStatus(this, 30);
			resetQueueChangedAnItem = resetQueueChangedAnItem || internal::DatabaseControlMessage::resetQueueStatus(this, 30);

			if (resetQueueChangedAnItem) {
				emit haveQueuedMessages();
			}
		}

		QString SimpleDatabase::getDefaultDatabaseFileName() {
			return QStringLiteral("openmittsu.sqlite");
		}

		QString SimpleDatabase::getCreateStatementForTable(Tables const& table) {
			QFile sqlFile;
			switch (table) {
				case Tables::ContactMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateContactMessages.sql"));
					break;
				case Tables::Contacts:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateContacts.sql"));
					break;
				case Tables::ControlMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateContactControlMessages.sql"));
					break;
				case Tables::GroupMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateGroupMessages.sql"));
					break;
				case Tables::FeatureLevels:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateFeatureLevels.sql"));
					break;
				case Tables::Groups:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateGroups.sql"));
					break;
				case Tables::Media:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateMedia.sql"));
					break;
				case Tables::Settings:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateSettings.sql"));
					break;
				case Tables::TableVersions:
					sqlFile.setFileName(QStringLiteral(":/sql/CreateTableVersions.sql"));
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unknown Table type given for fetching create statements from resources: " << static_cast<int>(table);
			}

			if (!sqlFile.exists() || !sqlFile.open(QFile::ReadOnly)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not find or open the file \"" << sqlFile.fileName().toStdString() << "\" containg the create statements from resources: " << static_cast<int>(table);
			} else {
				QTextStream fileStream(&sqlFile);

				QString result = fileStream.readAll();
				sqlFile.close();

				return result;
			}
		}

		QStringList SimpleDatabase::getUpdateStatementForTable(Tables const& table, int toVersion) {
			QFile sqlFile;
			switch (table) {
				case Tables::ContactMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateContactMessagesToVersion%1.sql").arg(toVersion));
					break;
				case Tables::Contacts:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateContactsToVersion%1.sql").arg(toVersion));
					break;
				case Tables::ControlMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateContactControlMessagesToVersion%1.sql").arg(toVersion));
					break;
				case Tables::GroupMessages:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateGroupMessagesToVersion%1.sql").arg(toVersion));
					break;
				case Tables::FeatureLevels:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateFeatureLevelsToVersion%1.sql").arg(toVersion));
					break;
				case Tables::Groups:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateGroupsToVersion%1.sql").arg(toVersion));
					break;
				case Tables::Media:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateMediaToVersion%1.sql").arg(toVersion));
					break;
				case Tables::Settings:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateSettingsToVersion%1.sql").arg(toVersion));
					break;
				case Tables::TableVersions:
					sqlFile.setFileName(QStringLiteral(":/sql/UpdateTableVersionsToVersion%1.sql").arg(toVersion));
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unknown Table type given for fetching update statements from resources: " << static_cast<int>(table);
			}

			if (!sqlFile.exists() || !sqlFile.open(QFile::ReadOnly)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not find or open the file \"" << sqlFile.fileName().toStdString() << "\" containg the update statements from resources: " << static_cast<int>(table);
			} else {
				QTextStream fileStream(&sqlFile);

				QString result = fileStream.readAll();
				sqlFile.close();

#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
				return result.split(QStringLiteral("__OPENMITTSU_QUERY_SEP__"), Qt::SkipEmptyParts);
#else
				return result.split(QStringLiteral("__OPENMITTSU_QUERY_SEP__"), QString::SkipEmptyParts);
#endif
			}
		}

		QString SimpleDatabase::getTableName(Tables const& table) {
			switch (table) {
				case Tables::ContactMessages:
					return QStringLiteral("contact_messages");
					break;
				case Tables::Contacts:
					return QStringLiteral("contacts");
					break;
				case Tables::ControlMessages:
					return QStringLiteral("control_messages");
					break;
				case Tables::FeatureLevels:
					return QStringLiteral("feature_levels");
					break;
				case Tables::GroupMessages:
					return QStringLiteral("group_messages");
					break;
				case Tables::Groups:
					return QStringLiteral("groups");
					break;
				case Tables::Media:
					return QStringLiteral("media");
					break;
				case Tables::Settings:
					return QStringLiteral("settings");
					break;
				case Tables::TableVersions:
					return QStringLiteral("table_versions");
					break;
				case Tables::SqliteMaster:
					return QStringLiteral("sqlite_master");
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "Unknown Table type given for fetching table name: " << static_cast<int>(table);
			}
		}

		bool SimpleDatabase::doesTableExist(Tables const& table) {
			QSqlQuery tableExistanceQuery(database);
			tableExistanceQuery.prepare("SELECT `name` FROM `sqlite_master` WHERE `type` = 'table' AND `name` = :tableName");

			QString const tableName(getTableName(table));
			tableExistanceQuery.bindValue(QStringLiteral(":tableName"), QVariant(tableName));

			if (tableExistanceQuery.exec() && tableExistanceQuery.isSelect()) {
				return tableExistanceQuery.next();
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not execute table existance query for table " << tableName.toStdString() << ".";
				return false;
			}
		}

		void SimpleDatabase::setTableVersion(Tables const& table, int tableVersion) {
			QSqlQuery query(database);
			QString const tableName = getTableName(table);

			query.prepare(QStringLiteral("SELECT `version` FROM `table_versions` WHERE `table_name` = :tableName"));
			query.bindValue(QStringLiteral(":tableName"), QVariant(tableName));
			if (!query.exec()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not query table version data for table " << tableName.toStdString() << " from 'table_versions'. Query error: " << query.lastError().text().toStdString();
			}

			if (!query.next()) {
				query.prepare(QStringLiteral("INSERT INTO `table_versions`(`table_name`,`version`) VALUES (:tableName, :tableVersion);"));
				query.bindValue(QStringLiteral(":tableName"), QVariant(tableName));
				query.bindValue(QStringLiteral(":tableVersion"), QVariant(tableVersion));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert table version data (" << tableName.toStdString() << ", " << tableVersion << ") into 'table_versions'. Query error: " << query.lastError().text().toStdString();
				}
			} else {
				query.prepare(QStringLiteral("UPDATE `table_versions` SET `version` = :tableVersion WHERE `table_name` = :tableName"));
				query.bindValue(QStringLiteral(":tableName"), QVariant(tableName));
				query.bindValue(QStringLiteral(":tableVersion"), QVariant(tableVersion));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not update table version data with (" << tableName.toStdString() << ", " << tableVersion << ") in 'table_versions'. Query error: " << query.lastError().text().toStdString();
				}
			}
		}

		int SimpleDatabase::getTableVersion(Tables const& table) {
			QSqlQuery query(database);
			QString const tableName = getTableName(table);

			query.prepare(QStringLiteral("SELECT `version` FROM `table_versions` WHERE `table_name` = :tableName"));
			query.bindValue(QStringLiteral(":tableName"), QVariant(tableName));
			if (!query.exec()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not query table version data for " << tableName.toStdString() << " from 'table_versions'. Query error: " << query.lastError().text().toStdString();
			}

			if (query.next()) {
				return query.value(0).toInt();
			}

			throw openmittsu::exceptions::InternalErrorException() << "Could not look up table version data for table '" << tableName.toStdString() << "'. Query error: " << query.lastError().text().toStdString();
		}

		int SimpleDatabase::createTableIfMissingAndGetVersion(Tables const& table, int createStatementVersion) {
			if (!doesTableExist(table)) {
				QSqlQuery query(database);
				if (!query.exec(getCreateStatementForTable(table))) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not create required table '" << getTableName(table).toStdString() << "'. Query error: " << query.lastError().text().toStdString();
				}

				setTableVersion(table, createStatementVersion);
			}

			int currentTableVersion = getTableVersion(table);
			return currentTableVersion;
		}

		void SimpleDatabase::createOrUpdateTables() {
			int versionTableVersions = createTableIfMissingAndGetVersion(Tables::TableVersions, 1);
			int versionTableContacts = createTableIfMissingAndGetVersion(Tables::Contacts, 1);
			int versionTableContactMessages = createTableIfMissingAndGetVersion(Tables::ContactMessages, 1);
			int versionTableControlMessages = createTableIfMissingAndGetVersion(Tables::ControlMessages, 1);
			int versionTableFeatureLevels = createTableIfMissingAndGetVersion(Tables::FeatureLevels, 1);
			int versionTableGroups = createTableIfMissingAndGetVersion(Tables::Groups, 1);
			int versionTableGroupMessages = createTableIfMissingAndGetVersion(Tables::GroupMessages, 1);
			int versionTableMedia = createTableIfMissingAndGetVersion(Tables::Media, 3);
			int versionTableSettings = createTableIfMissingAndGetVersion(Tables::Settings, 1);

			if (versionTableVersions != 1) {
				LOGGER()->warn("Table TableVersions has version {} instead of {}.", versionTableVersions, 1);
			}
			if (versionTableContacts != 1) {
				LOGGER()->warn("Table Contacts has version {} instead of {}.", versionTableContacts, 1);
			}
			if (versionTableContactMessages != 1) {
				LOGGER()->warn("Table ContactMessages has version {} instead of {}.", versionTableContactMessages, 1);
			}
			if (versionTableControlMessages != 1) {
				LOGGER()->warn("Table ControlMessages has version {} instead of {}.", versionTableControlMessages, 1);
			}
			if (versionTableFeatureLevels != 1) {
				LOGGER()->warn("Table FeatureLevels has version {} instead of {}.", versionTableFeatureLevels, 1);
			}
			if (versionTableGroups != 1) {
				LOGGER()->warn("Table Groups has version {} instead of {}.", versionTableGroups, 1);
			}
			if (versionTableGroupMessages != 1) {
				LOGGER()->warn("Table GroupMessages has version {} instead of {}.", versionTableGroupMessages, 1);
			}
			if (versionTableMedia != 3) {
				LOGGER()->warn("Table Media has version {} instead of {}.", versionTableMedia, 3);

				if (versionTableMedia == 1) {
					// Update 1: Added `type` field to media table.
					LOGGER()->info("Upgrading media database to file schema version 2...");
					this->transactionStart();
					QSqlQuery query(database);
					QStringList const updateQueries = getUpdateStatementForTable(Tables::Media, 2);
					auto it = updateQueries.constBegin();
					auto const end = updateQueries.constEnd();
					for (; it != end; ++it) {
						LOGGER_DEBUG("Running part of update query: {}", it->toStdString());
						if (!query.exec(*it)) {
							throw openmittsu::exceptions::InternalErrorException() << "Could not update table 'media' to version 2. Query error: " << query.lastError().text().toStdString();
						}
					}
					setTableVersion(Tables::Media, 2);
					m_mediaFileStorage.upgradeMediaDatabase(1);
					this->transactionCommit();
					LOGGER()->info("Upgrading media database to file schema version 2... Done.");
				} else if (versionTableMedia == 2) {
					// Update 2: Creation of the tables was broken, everyone maybe needs this fix.
					LOGGER()->info("Upgrading media database to file schema version 3...");
					this->transactionStart();
					QSqlQuery query(database);
					// Two possible situations: All good, or: uid is UNIQUE and the primary key is not fixed.
					query.prepare(QStringLiteral("SELECT `sql` FROM `sqlite_master` WHERE `type` = \"table\" AND `tbl_name` = \"media\""));
					if (query.exec() && query.isSelect() && query.next()) {
						QString const sqlCmd = query.value(QStringLiteral("sql")).toString();
						QRegularExpression keyExpr("PRIMARY\\s+KEY\\s*\\(\\s*`?uid`?\\s*\\)");
						if (sqlCmd.contains(QStringLiteral("UNIQUE")) || sqlCmd.contains(keyExpr)) {
							LOGGER()->warn("Database is suffering from the incorrect creation bug, fixing...");
							QStringList const updateQueries = getUpdateStatementForTable(Tables::Media, 3);
							auto it = updateQueries.constBegin();
							auto const end = updateQueries.constEnd();
							for (; it != end; ++it) {
								LOGGER_DEBUG("Running part of update query: {}", it->toStdString());
								if (!query.exec(*it)) {
									throw openmittsu::exceptions::InternalErrorException() << "Could not update table 'media' to version 3. Query error: " << query.lastError().text().toStdString();
								}
							}
						} else {
							LOGGER()->info("Database is NOT suffering from the incorrect creation bug.");
						}
					} else {
						throw openmittsu::exceptions::InternalErrorException() << "Could not execute media table creation command query. Query error: " << query.lastError().text().toStdString();
					}
					
					setTableVersion(Tables::Media, 3);
					m_mediaFileStorage.upgradeMediaDatabase(2);
					this->transactionCommit();
					LOGGER()->info("Upgrading media database to file schema version 3... Done.");
				}
			}
			if (versionTableSettings != 1) {
				LOGGER()->warn("Table Settings has version {} instead of {}.", versionTableSettings, 1);
			}

			// Updates:
			// Update 1: Added `type` field to media table.
		}

		QString SimpleDatabase::generateUuid() const {
			QUuid const uuid = QUuid::createUuid();
	
			QString const uuidString(uuid.toString());
			if (uuidString.startsWith('{')) {
				return uuidString.mid(1, uuidString.length() - 2);
			}

			return uuidString;
		}

		openmittsu::protocol::MessageId SimpleDatabase::getNextMessageId(openmittsu::protocol::ContactId const& contact) {
			openmittsu::protocol::MessageId id(openmittsu::protocol::MessageId::random());

			while (true) {
				if (!internal::DatabaseContactMessage::exists(this, contact, id) && !internal::DatabaseControlMessage::exists(this, contact, id)) {
					break;
				}

				id = openmittsu::protocol::MessageId::random();
			}

			return id;
		}

		openmittsu::protocol::MessageId SimpleDatabase::getNextMessageId(openmittsu::protocol::GroupId const& group) {
			openmittsu::protocol::MessageId id(openmittsu::protocol::MessageId::random());

			while (true) {
				if (!internal::DatabaseGroupMessage::exists(this, group, id)) {
					break;
				}

				id = openmittsu::protocol::MessageId::random();
			}

			return id;
		}

		QString SimpleDatabase::makeBodyForFile(QByteArray const& file, QString const& mimeType, QString const& fileName, QString const& caption) const {
			// [null,null,""image/gif"",481500,""bla.gif"",1,true,""Test""]
			QString body;
			if (caption.isEmpty()) {
				body = QStringLiteral("[null,null,\"%1\",%2,\"%3\",1,true,null]").arg(mimeType).arg(file.size()).arg(fileName);
			} else {
				body = QStringLiteral("[null,null,\"%1\",%2,\"%3\",1,true,\"%4\"]").arg(mimeType).arg(file.size()).arg(fileName).arg(caption);
			}
			return body;
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageAudio(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!hasContact(receiver)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact audio message, the given contact " << receiver.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, audio, MediaFileType::TYPE_STANDARD);
			} while (!result);			
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, uuid, timeCreated, ContactMessageType::AUDIO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageFile(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!hasContact(receiver)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact file message, the given contact " << receiver.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, file, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, uuid, timeCreated, ContactMessageType::FILE, makeBodyForFile(file, mimeType, fileName, caption), isQueued, false, caption);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageVideo(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!hasContact(receiver)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact video message, the given contact " << receiver.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, video, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, uuid, timeCreated, ContactMessageType::VIDEO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, generateUuid(), timeCreated, ContactMessageType::TEXT, message, isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			if (!hasContact(receiver)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact image message, the given contact " << receiver.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
			} while (!result);
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, uuid, timeCreated, ContactMessageType::IMAGE, QStringLiteral(""), isQueued, false, caption);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			return internal::DatabaseContactMessage::insertContactMessageFromUs(this, receiver, generateUuid(), timeCreated, ContactMessageType::LOCATION, location.toDatabaseString(), isQueued, false, location.getDescription());
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageAudio(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group audio message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, audio, MediaFileType::TYPE_STANDARD);
			} while (!result);
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, uuid, timeCreated, GroupMessageType::AUDIO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageFile(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group file message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, file, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, uuid, timeCreated, GroupMessageType::FILE, makeBodyForFile(file, mimeType, fileName, caption), isQueued, false, caption);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group video message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, video, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, uuid, timeCreated, GroupMessageType::VIDEO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::TEXT, message, isQueued, false, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group image message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
			} while (!result);
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, uuid, timeCreated, GroupMessageType::IMAGE, QStringLiteral(""), isQueued, false, caption);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::LOCATION, location.toDatabaseString(), isQueued, false, location.getDescription());
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
			return internal::DatabaseControlMessage::insertControlMessageFromUs(this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::RECEIVED);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
			return internal::DatabaseControlMessage::insertControlMessageFromUs(this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::READ);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
			return internal::DatabaseControlMessage::insertControlMessageFromUs(this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::AGREE);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
			return internal::DatabaseControlMessage::insertControlMessageFromUs(this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::DISAGREE);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			// not stored
			return getNextMessageId(receiver);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			// not stored
			return getNextMessageId(receiver);
		}

		void SimpleDatabase::storeReceivedContactMessageAudio(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& audio, quint16 lengthInSeconds) {
			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, audio, MediaFileType::TYPE_STANDARD);
			} while (!result);
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, uuid, timeSent, timeReceived, ContactMessageType::AUDIO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedContactMessageFile(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, file, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, uuid, timeSent, timeReceived, ContactMessageType::FILE, makeBodyForFile(file, mimeType, fileName, caption), false, caption);
		}

		void SimpleDatabase::storeReceivedContactMessageVideo(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, video, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, uuid, timeSent, timeReceived, ContactMessageType::VIDEO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, generateUuid(), timeSent, timeReceived, ContactMessageType::TEXT, message, false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
			} while (!result);
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, uuid, timeSent, timeReceived, ContactMessageType::IMAGE, QStringLiteral(""), false, caption);
		}

		void SimpleDatabase::storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			internal::DatabaseContactMessage::insertContactMessageFromThem(this, sender, messageId, generateUuid(), timeSent, timeReceived, ContactMessageType::LOCATION, location.toDatabaseString(), false, location.getDescription());
		}

		void SimpleDatabase::storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			try {
				internal::DatabaseContactMessage message(this, sender, referredMessageId);
				message.setMessageState(UserMessageState::DELIVERED, timeSent);
			} catch (openmittsu::exceptions::InternalErrorExceptionImpl& iee) {
				LOGGER()->warn("Could not saved received \"received\" receipt: {}", iee.what());
			}
		}

		void SimpleDatabase::storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			try {
				internal::DatabaseContactMessage message(this, sender, referredMessageId);
				message.setMessageState(UserMessageState::READ, timeSent);
			} catch (openmittsu::exceptions::InternalErrorExceptionImpl& iee) {
				LOGGER()->warn("Could not saved received \"seen\" receipt: {}", iee.what());
			}
		}

		void SimpleDatabase::storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			try {
				internal::DatabaseContactMessage message(this, sender, referredMessageId);
				message.setMessageState(UserMessageState::USERACK, timeSent);
			} catch (openmittsu::exceptions::InternalErrorExceptionImpl& iee) {
				LOGGER()->warn("Could not saved received \"agree\" receipt: {}", iee.what());
			}
		}

		void SimpleDatabase::storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
			try {
				internal::DatabaseContactMessage message(this, sender, referredMessageId);
				message.setMessageState(UserMessageState::USERDEC, timeSent);
			} catch (openmittsu::exceptions::InternalErrorExceptionImpl& iee) {
				LOGGER()->warn("Could not saved received \"disagree\" receipt: {}", iee.what());
			}
		}

		void SimpleDatabase::storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!hasContact(sender)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact typing notfication, the given contact " << sender.toString() << " is unknown!";
			}
	
			emit contactStartedTyping(sender);
		}

		void SimpleDatabase::storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
			if (!hasContact(sender)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save contact typing notfication, the given contact " << sender.toString() << " is unknown!";
			}
	
			emit contactStoppedTyping(sender);
		}

		void SimpleDatabase::storeReceivedGroupMessageAudio(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& audio, quint16 lengthInSeconds) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group audio message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, audio, MediaFileType::TYPE_STANDARD);
			} while (!result);
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::AUDIO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedGroupMessageFile(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& file, QByteArray const& coverImage, QString const& mimeType, QString const& fileName, QString const& caption) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group file message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, file, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::FILE, makeBodyForFile(file, mimeType, fileName, caption), false, caption);
		}

		void SimpleDatabase::storeReceivedGroupMessageVideo(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& video, QByteArray const& coverImage, quint16 lengthInSeconds) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group video message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, video, MediaFileType::TYPE_STANDARD) && insertMediaItem(uuid, coverImage, MediaFileType::TYPE_THUMBNAIL);
			} while (!result);
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::VIDEO, QStringLiteral("[%1,true,\"\",\"\"]").arg(lengthInSeconds), false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::TEXT, message, false, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not save group image message, the given group " << group.toString() << " is unknown!";
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
			} while (!result);
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::IMAGE, QStringLiteral(""), false, caption);
		}

		void SimpleDatabase::storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::LOCATION, location.toDatabaseString(), false, location.getDescription());
		}

		void SimpleDatabase::storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group leave request!";
			}

			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::LEAVE, QStringLiteral(""), true, QStringLiteral(""));

			QSet<openmittsu::protocol::ContactId> currentMembers = m_contactAndGroupDataProvider.getGroupMembers(group, false);
			currentMembers.remove(sender);
			m_contactAndGroupDataProvider.setGroupMembers(group, currentMembers);
		}

		void SimpleDatabase::storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group sync request!";
			} else if (!(group.getOwner() == getSelfContact())) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not store group sync request from " << sender.toString() << "!";
			}

			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::SYNC_REQUEST, QStringLiteral(""), true, QStringLiteral(""));
		}

		void SimpleDatabase::storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& title) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group title!";
			} else if (!(group.getOwner() == sender)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by " << sender.toString() << ", can not set group title!";
			} else {
				internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::SET_TITLE, title, true, QStringLiteral(""));
				m_contactAndGroupDataProvider.setGroupTitle(group, title);
			}
		}

		void SimpleDatabase::storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group image!";
			} else if (!(group.getOwner() == sender)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by " << sender.toString() << ", can not set group image!";
			} else {
				QString uuid;
				bool result = false;
				do {
					uuid = generateUuid();
					result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
				} while (!result);
				internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::SET_IMAGE, QStringLiteral(""), true, QStringLiteral(""));
				m_contactAndGroupDataProvider.setGroupImage(group, image);
			}
		}

		void SimpleDatabase::storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
			if (!(group.getOwner() == sender)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by " << group.getOwner().toString() << ", can not store group group creation from " << sender.toString() << "!";
			}

			QString result = "";
			for (openmittsu::protocol::ContactId const& c : members) {
				if (!result.isEmpty()) {
					result.append(QStringLiteral(";"));
				}
				result.append(c.toQString());
			}

			internal::DatabaseGroupMessage::insertGroupMessageFromThem(this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::GROUP_CREATION, result, true, QStringLiteral(""));
	
			QVector<NewGroupData> vector;
			NewGroupData data(group, "", timeSent, members, false, false);
			vector.append(data);
			m_contactAndGroupDataProvider.addGroup(vector);
		}

		void SimpleDatabase::storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (internal::DatabaseControlMessage::exists(this, receiver, messageId)) {
				internal::DatabaseControlMessage message(internal::DatabaseControlMessage::fromReceiverAndControlMessageId(this, receiver, messageId));
				message.setMessageState(ControlMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
			} else {
				internal::DatabaseContactMessage message(this, receiver, messageId);
				message.setMessageState(UserMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
			}
		}

		void SimpleDatabase::storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
			if (internal::DatabaseControlMessage::exists(this, receiver, messageId)) {
				internal::DatabaseControlMessage message(internal::DatabaseControlMessage::fromReceiverAndControlMessageId(this, receiver, messageId));
				message.setMessageState(ControlMessageState::SENT, openmittsu::protocol::MessageTime::now());
			} else {
				internal::DatabaseContactMessage message(this, receiver, messageId);
				message.setMessageState(UserMessageState::SENT, openmittsu::protocol::MessageTime::now());
			}
		}

		void SimpleDatabase::storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			internal::DatabaseGroupMessage message(this, group, messageId);
			message.setMessageState(UserMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
		}

		void SimpleDatabase::storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
			internal::DatabaseGroupMessage message(this, group, messageId);
			message.setMessageState(UserMessageState::SENT, openmittsu::protocol::MessageTime::now());
		}

		void SimpleDatabase::storeContactMessagesFromBackup(QList<openmittsu::backup::ContactMessageBackupObject> const& messages) {
			internal::DatabaseContactMessage::insertContactMessagesFromBackup(this, messages);
		}

		void SimpleDatabase::storeGroupMessagesFromBackup(QList<openmittsu::backup::GroupMessageBackupObject> const& messages) {
			internal::DatabaseGroupMessage::insertGroupMessagesFromBackup(this, messages);
		}

		void SimpleDatabase::storeContactMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) {
			m_mediaFileStorage.insertMediaItemsFromBackup(items);
		}

		void SimpleDatabase::storeGroupMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) {
			m_mediaFileStorage.insertMediaItemsFromBackup(items);
		}

		void SimpleDatabase::storeNewContact(QVector<NewContactData> const& newContactData) {
			m_contactAndGroupDataProvider.addContact(newContactData);
		}

		void SimpleDatabase::storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
			QVector<NewContactData> vector;
			NewContactData data(contact, publicKey);
			vector.append(data);
			storeNewContact(vector);
		}

		void SimpleDatabase::storeNewGroup(QVector<NewGroupData> const& newGroupData) {
			QSet<openmittsu::protocol::ContactId> knownContacts;
			for (NewGroupData const& newGroup : newGroupData) {
				for (openmittsu::protocol::ContactId const& c : newGroup.members) {
					if (knownContacts.contains(c)) {
						continue;
					} else if (!hasContact(c)) {
						throw openmittsu::exceptions::InternalErrorException() << "The given group " << newGroup.id.toString() << " contains member " << c.toString() << " that is unknown.";
					}
					knownContacts.insert(c);
				}
			}

			m_contactAndGroupDataProvider.addGroup(newGroupData);
		}

		void SimpleDatabase::storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) {
			QVector<NewGroupData> vector;
			NewGroupData data(groupId, members, isAwaitingSync);
			vector.append(data);
			storeNewGroup(vector);
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) {
			if (!(group.getOwner() == getSelfContact())) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not store group creation from us!";
			}

			QString memberString;
			for (openmittsu::protocol::ContactId const& contactId : members) {
				if (!hasContact(contactId)) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not add group " << group.toString() << ", the given member identity " << contactId.toString() << " is unknown!";
				}

				if (memberString.isEmpty()) {
					memberString = contactId.toQString();
				} else {
					memberString.append(";").append(contactId.toQString());
				}
			}

			if (apply) {
				QVector<NewGroupData> vector;
				NewGroupData data(group, "", timeCreated, members, false, false);
				vector.append(data);
				m_contactAndGroupDataProvider.addGroup(vector);
			}

			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::GROUP_CREATION, memberString, isQueued, true, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group title!";
			} else if (!(group.getOwner() == getSelfContact())) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not set group title!";
			}
	
			if (apply) {
				m_contactAndGroupDataProvider.setGroupTitle(group, groupTitle);
			}

			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::SET_TITLE, groupTitle, isQueued, true, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group image!";
			} else if (!(group.getOwner() == getSelfContact())) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not set group image!";
			}

			if (apply) {
				m_contactAndGroupDataProvider.setGroupImage(group, image);
			}

			QString uuid;
			bool result = false;
			do {
				uuid = generateUuid();
				result = insertMediaItem(uuid, image, MediaFileType::TYPE_STANDARD);
			} while (!result);
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, uuid, timeCreated, GroupMessageType::SET_IMAGE, QStringLiteral(""), isQueued, true, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
			if (group.getOwner() == getSelfContact()) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by us, can not store group sync request from us!";
			} else if (!hasContact(group.getOwner())) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by " << group.getOwner().toString() << ", which is unknown!";
			}
	
			return internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::SYNC_REQUEST, QStringLiteral(""), isQueued, true, QStringLiteral(""));
		}

		openmittsu::protocol::MessageId SimpleDatabase::storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group leave request!";
			} else if (group.getOwner() == getSelfContact()) {
				throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by us, can not store group leave request from us!";
			}

			openmittsu::protocol::MessageId const messageId = internal::DatabaseGroupMessage::insertGroupMessageFromUs(this, group, generateUuid(), timeCreated, GroupMessageType::LEAVE, QStringLiteral(""), isQueued, true, QStringLiteral(""));
	
			if (apply) {
				QSet<openmittsu::protocol::ContactId> currentMembers = m_contactAndGroupDataProvider.getGroupMembers(group, false);
				currentMembers.remove(m_selfContact);
				m_contactAndGroupDataProvider.setGroupMembers(group, currentMembers);
			}

			return messageId;
		}

		bool SimpleDatabase::hasContact(openmittsu::protocol::ContactId const& identity) const {
			return m_contactAndGroupDataProvider.hasContact(identity);
		}

		bool SimpleDatabase::hasGroup(openmittsu::protocol::GroupId const& group) const {
			return m_contactAndGroupDataProvider.hasGroup(group);
		}

		bool SimpleDatabase::isDeleteted(openmittsu::protocol::GroupId const& group) const {
			return m_contactAndGroupDataProvider.getGroupStatus(group) == openmittsu::protocol::GroupStatus::DELETED;
		}

		openmittsu::protocol::GroupStatus SimpleDatabase::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
			if (hasGroup(group)) {
				if (m_contactAndGroupDataProvider.getGroupIsAwaitingSync(group)) {
					return openmittsu::protocol::GroupStatus::TEMPORARY;
				} else {
					return openmittsu::protocol::GroupStatus::KNOWN;
				}
			} else if (isDeleteted(group)) {
				return openmittsu::protocol::GroupStatus::DELETED;
			} else {
				return openmittsu::protocol::GroupStatus::UNKNOWN;
			}
		}

		openmittsu::protocol::ContactStatus SimpleDatabase::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
			if (hasContact(contact)) {
				return openmittsu::protocol::ContactStatus::KNOWN;
			} else {
				return openmittsu::protocol::ContactStatus::UNKNOWN;
			}
		}

		QString SimpleDatabase::getOptionValueInternal(QString const& optionName, bool isInternalOption) {
			QSqlQuery query(database);
			query.prepare(QStringLiteral("SELECT `value` FROM `settings` WHERE `is_internal` = :isInternal AND `name` = :name"));
			query.bindValue(QStringLiteral(":name"), QVariant(optionName));
			query.bindValue(QStringLiteral(":isInternal"), QVariant(((isInternalOption) ? 1 : 0)));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not fetch option " << optionName.toStdString() << " from settings. Query error: " << query.lastError().text().toStdString();
			} else if (!query.next()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not fetch option " << optionName.toStdString() << " from settings - it does not exist.";
			} else {
				return query.value(QStringLiteral("value")).toString();
			}
		}

		bool SimpleDatabase::hasOptionInternal(QString const& optionName, bool isInternalOption) {
			QSqlQuery query(database);
			query.prepare(QStringLiteral("SELECT `value` FROM `settings` WHERE `is_internal` = :isInternal AND `name` = :name"));
			query.bindValue(QStringLiteral(":name"), QVariant(optionName));
			query.bindValue(QStringLiteral(":isInternal"), QVariant(((isInternalOption) ? 1 : 0)));

			if (query.exec() && query.isSelect()) {
				return query.next();
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not fetch option " << optionName.toStdString() << " from settings. Query error: " << query.lastError().text().toStdString();
			}
		}

		void SimpleDatabase::setOptionInternal(QString const& optionName, QString const& optionValue, bool isInternalOption) {
			if (hasOptionInternal(optionName, isInternalOption)) {
				QSqlQuery query(database);
				query.prepare(QStringLiteral("UPDATE `settings` SET `value` = :newValue WHERE `name` = :name AND `is_internal` = :isInternal;"));
				query.bindValue(QStringLiteral(":name"), QVariant(optionName));
				query.bindValue(QStringLiteral(":newValue"), QVariant(optionValue));
				query.bindValue(QStringLiteral(":isInternal"), QVariant((isInternalOption) ? 1 : 0));
		
				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not update settings value in 'settings'. Query error: " << query.lastError().text().toStdString();
				}
			} else {
				QSqlQuery query(database);
				query.prepare(QStringLiteral("INSERT INTO `settings` (`name`, `is_internal`, `value`) VALUES (:name, :isInternal, :newValue);"));
				query.bindValue(QStringLiteral(":name"), QVariant(optionName));
				query.bindValue(QStringLiteral(":newValue"), QVariant(optionValue));
				query.bindValue(QStringLiteral(":isInternal"), QVariant((isInternalOption) ? 1 : 0));

				if (!query.exec()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not insert settings value in 'settings'. Query error: " << query.lastError().text().toStdString();
				}
			}
		}

		bool SimpleDatabase::hasOption(QString const& optionName) {
			return hasOptionInternal(optionName, false);
		}

		OptionNameToValueMap SimpleDatabase::getOptions() {
			OptionNameToValueMap result;
			QSqlQuery query(database);
			query.prepare(QStringLiteral("SELECT `name`, `value` FROM `settings` WHERE `is_internal` = 0;"));

			if (!query.exec() || !query.isSelect()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not enumerate options from settings. Query error: " << query.lastError().text().toStdString();
			}
			
			while (query.next()) {
				result.insert(query.value(QStringLiteral("name")).toString(), query.value(QStringLiteral("value")).toString());
			}

			return result;
		}

		void SimpleDatabase::setOptions(OptionNameToValueMap const& options) {
			auto it = options.constBegin();
			auto const end = options.constEnd();
			for (; it != end; ++it) {
				setOptionInternal(it.key(), it.value(), false);
			}

			emit optionsChanged();
		}

		QString SimpleDatabase::getOptionValueAsString(QString const& optionName) {
			return getOptionValueInternal(optionName, false);
		}

		bool SimpleDatabase::getOptionValueAsBool(QString const& optionName) {
			QString const value = getOptionValueInternal(optionName, false);
			if (value == QStringLiteral("0")) {
				return false;
			} else if (value == QStringLiteral("1")) {
				return true;
			} else {
				throw openmittsu::exceptions::InternalErrorException() << "Could not convert value \"" << value.toStdString() << "\" of option " << optionName.toStdString() << " into a boolean.";
			}
		}

		QByteArray SimpleDatabase::getOptionValueAsByteArray(QString const& optionName) {
			QString const value = getOptionValueInternal(optionName, false);
			QRegularExpression regexp("^(?:[a-f0-9][a-f0-9])*$");
			QRegularExpressionMatch match = regexp.match(value);
			if (!match.hasMatch()) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not convert value \"" << value.toStdString() << "\" of option " << optionName.toStdString() << " into a boolean.";
			}

			return QByteArray::fromHex(value.toUtf8());
		}

		void SimpleDatabase::setOptionValue(QString const& optionName, QString const& optionValue) {
			setOptionInternal(optionName, optionValue, false);
		}

		void SimpleDatabase::setOptionValue(QString const& optionName, bool const& optionValue) {
			if (optionValue) {
				setOptionInternal(optionName, QStringLiteral("1"), false);
			} else {
				setOptionInternal(optionName, QStringLiteral("0"), false);
			}
		}

		void SimpleDatabase::setOptionValue(QString const& optionName, QByteArray const& optionValue) {
			setOptionInternal(optionName, QString(optionValue.toHex()), false);
		}

		MediaFileItem SimpleDatabase::getMediaItem(QString const& uuid, MediaFileType const& fileType) const {
			return m_mediaFileStorage.getMediaItem(uuid, fileType);
		}

		bool SimpleDatabase::insertMediaItem(QString const& uuid, QByteArray const& data, MediaFileType const& fileType) {
			try {
				m_mediaFileStorage.insertMediaItem(uuid, data, fileType);
			} catch (openmittsu::exceptions::UuidAlreadyExistsExceptionImpl&) {
				LOGGER()->error("Detected UUID duplication when inserting file with UUID {}, size {} and file type {} (if this message appears multiple times, something major is broken!).", uuid.toStdString(), data.size(), MediaFileTypeHelper::toInt(fileType));
				return false;
			}
			return true;
		}

		void SimpleDatabase::removeMediaItem(QString const& uuid, MediaFileType const& fileType) {
			m_mediaFileStorage.removeMediaItem(uuid, fileType);
		}

		void SimpleDatabase::removeAllMediaItems(QString const& uuid) {
			m_mediaFileStorage.removeAllMediaItems(uuid);
		}

		std::shared_ptr<openmittsu::backup::IdentityBackup> SimpleDatabase::getBackup() const {
			return std::make_shared<openmittsu::backup::IdentityBackup>(*m_identityBackup);
		}

		bool SimpleDatabase::isServerGroupKnownAndNotOutdated(int maximalAgeInSeconds) {
			if (!hasOptionInternal(QStringLiteral("serverGroupLastFetched"), true)) {
				return false;
			}
			QString const serverGroupLastFetchedString = getOptionValueInternal(QStringLiteral("serverGroupLastFetched"), true);
			bool ok = false;
			qint64 const serverGroupLastFetched = serverGroupLastFetchedString.toLongLong(&ok);
			if (!ok) {
				LOGGER()->error("Failed to parse 'serverGroupLastFetched' from value '{}'!", serverGroupLastFetchedString.toStdString());
				return false;
			}

			return (QDateTime::currentDateTime().toSecsSinceEpoch() - serverGroupLastFetched) <= maximalAgeInSeconds;
		}
		
		void SimpleDatabase::setServerGroup(QString const& serverGroup) {
			setOptionInternal(QStringLiteral("serverGroup"), serverGroup, true);
			setOptionInternal(QStringLiteral("serverGroupLastFetched"), QString::number(QDateTime::currentDateTime().toSecsSinceEpoch()), true);
		}
		
		QString SimpleDatabase::getServerGroup() {
			if (!hasOptionInternal(QStringLiteral("serverGroup"), true)) {
				return QString();
			}
			return getOptionValueInternal(QStringLiteral("serverGroup"), true);
		}

		void SimpleDatabase::updateCachedIdentityBackup() {
			QString const identityBackup = getOptionValueInternal(QStringLiteral("identity"), true);
			m_identityBackup = std::make_unique<openmittsu::backup::IdentityBackup>(openmittsu::backup::IdentityBackup::fromBackupString(identityBackup, m_password));
		}

		openmittsu::protocol::ContactId SimpleDatabase::getSelfContact() const {
			return m_selfContact;
		}

		void SimpleDatabase::setBackup(openmittsu::protocol::ContactId selfId, openmittsu::crypto::KeyPair key) {
			openmittsu::backup::IdentityBackup backupData(selfId, key);
			setOptionInternal(QStringLiteral("identity"), backupData.toBackupString(m_password), true);
		}

		openmittsu::crypto::PublicKey SimpleDatabase::getContactPublicKey(openmittsu::protocol::ContactId const& identity) const {
			return m_contactAndGroupDataProvider.getPublicKey(identity);
		}

		int SimpleDatabase::getContactCount() const {
			return m_contactAndGroupDataProvider.getContactCount();
		}

		int SimpleDatabase::getGroupCount() const {
			return m_contactAndGroupDataProvider.getGroupCount();
		}

		int SimpleDatabase::getContactMessageCount() const {
			return openmittsu::database::internal::DatabaseContactMessage::getContactMessageCount(this);
		}

		int SimpleDatabase::getGroupMessageCount() const {
			return openmittsu::database::internal::DatabaseGroupMessage::getGroupMessageCount(this);
		}

		int SimpleDatabase::getMediaItemCount() const {
			return m_mediaFileStorage.getMediaItemCount();
		}

		void SimpleDatabase::setContactFirstName(openmittsu::protocol::ContactId const& identity, QString const& firstName) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setFirstName(identity, firstName);
			}
		}

		void SimpleDatabase::setContactLastName(openmittsu::protocol::ContactId const& identity, QString const& lastName) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setLastName(identity, lastName);
			}
		}

		void SimpleDatabase::setContactNickName(openmittsu::protocol::ContactId const& identity, QString const& nickname) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setNickName(identity, nickname);
			}
		}

		void SimpleDatabase::setContactAccountStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::AccountStatus const& status) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setAccountStatus(identity, status);
			}
		}

		void SimpleDatabase::setContactVerificationStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setVerificationStatus(identity, verificationStatus);
			}
		}

		void SimpleDatabase::setContactFeatureLevel(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::FeatureLevel const& featureLevel) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setFeatureLevel(identity, featureLevel);
			}
		}

		void SimpleDatabase::setContactColor(openmittsu::protocol::ContactId const& identity, int color) {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
			} else {
				m_contactAndGroupDataProvider.setColor(identity, color);
			}
		}

		void SimpleDatabase::setContactAccountStatusBatch(ContactToAccountStatusMap const& status) {
			m_contactAndGroupDataProvider.setAccountStatusBatch(status);
		}

		void SimpleDatabase::setContactFeatureLevelBatch(ContactToFeatureLevelMap const& featureLevels) {
			m_contactAndGroupDataProvider.setFeatureLevelBatch(featureLevels);
		}

		QSet<openmittsu::protocol::ContactId> SimpleDatabase::getKnownContacts() const {
			return m_contactAndGroupDataProvider.getKnownContacts();
		}

		QSet<openmittsu::protocol::ContactId> SimpleDatabase::getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const {
			return m_contactAndGroupDataProvider.getContactsRequiringFeatureLevelCheck(maximalAgeInSeconds);
		}

		QSet<openmittsu::protocol::ContactId> SimpleDatabase::getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const {
			return m_contactAndGroupDataProvider.getContactsRequiringAccountStatusCheck(maximalAgeInSeconds);
		}

		QSet<openmittsu::protocol::GroupId> SimpleDatabase::getKnownGroups() const {
			return m_contactAndGroupDataProvider.getKnownGroups();
		}

		QSet<openmittsu::protocol::ContactId> SimpleDatabase::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
			openmittsu::protocol::GroupStatus const status = m_contactAndGroupDataProvider.getGroupStatus(group);
			if ((status != openmittsu::protocol::GroupStatus::KNOWN) && (status != openmittsu::protocol::GroupStatus::TEMPORARY)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not get group members, the given group " << group.toString() << " is unknown!";
			}

			return m_contactAndGroupDataProvider.getGroupMembers(group, excludeSelfContact);
		}

		QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> SimpleDatabase::getKnownContactsWithPublicKeys() const {
			return m_contactAndGroupDataProvider.getKnownContactsWithPublicKeys();
		}

		QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> SimpleDatabase::getKnownGroupsWithMembersAndTitles() const {
			return m_contactAndGroupDataProvider.getKnownGroupsWithMembersAndTitles();
		}

		QHash<openmittsu::protocol::GroupId, QString> SimpleDatabase::getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const {
			if (!hasContact(identity)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not get groups containing contact because the given contact " << identity.toString() << " is unknown!";
			}

			return m_contactAndGroupDataProvider.getKnownGroupsContainingMember(identity);
		}

		openmittsu::protocol::MessageTime SimpleDatabase::getGroupLastSyncRequestTime(openmittsu::protocol::GroupId const& group) {
			return internal::DatabaseGroupMessageCursor::getLastSyncRequestTimeByUs(this, group);
		}

		internal::DatabaseContactMessageCursor SimpleDatabase::getMessageCursor(openmittsu::protocol::ContactId const& contact) {
			if (!hasContact(contact)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not create message cursor because the given contact " << contact.toString() << " is unknown!";
			}
			return internal::DatabaseContactMessageCursor(this, contact);
		}

		internal::DatabaseGroupMessageCursor SimpleDatabase::getMessageCursor(openmittsu::protocol::GroupId const& group) {
			if (!hasGroup(group)) {
				throw openmittsu::exceptions::InternalErrorException() << "Could not create message cursor because the given group " << group.toString() << " is unknown!";
			}
			return internal::DatabaseGroupMessageCursor(this, group);
		}

		void SimpleDatabase::deleteContactMessageByUuid(openmittsu::protocol::ContactId const& contact, QString const& uuid) {
			internal::DatabaseContactMessageCursor cursor(this, contact, uuid);
			cursor.deleteMessage(true);
		}
		
		void SimpleDatabase::deleteContactMessagesByAge(openmittsu::protocol::ContactId const& contact, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) {
			internal::DatabaseContactMessageCursor::deleteMessagesByAge(this, contact, olderThanOrNewerThan, timePoint);
		}
		
		void SimpleDatabase::deleteContactMessagesByCount(openmittsu::protocol::ContactId const& contact, bool oldestOrNewest, int count) {
			internal::DatabaseContactMessageCursor::deleteMessagesByCount(this, contact, oldestOrNewest, count);
		}

		void SimpleDatabase::deleteGroupMessageByUuid(openmittsu::protocol::GroupId const& group, QString const& uuid) {
			internal::DatabaseGroupMessageCursor cursor(this, group, uuid);
			cursor.deleteMessage(true);
		}

		void SimpleDatabase::deleteGroupMessagesByAge(openmittsu::protocol::GroupId const& group, bool olderThanOrNewerThan, openmittsu::protocol::MessageTime const& timePoint) {
			internal::DatabaseGroupMessageCursor::deleteMessagesByAge(this, group, olderThanOrNewerThan, timePoint);
		}

		void SimpleDatabase::deleteGroupMessagesByCount(openmittsu::protocol::GroupId const& group, bool oldestOrNewest, int count) {
			internal::DatabaseGroupMessageCursor::deleteMessagesByCount(this, group, oldestOrNewest, count);
		}

		void SimpleDatabase::announceMessageChanged(QString const& uuid) {
			LOGGER_DEBUG("Database: Announcing messageChanged() for UUID {}.", uuid.toStdString());
			emit messageChanged(uuid);
		}

		void SimpleDatabase::announceMessageDeleted(QString const& uuid) {
			LOGGER_DEBUG("Database: Announcing messageDeleted() for UUID {}.", uuid.toStdString());
			emit messageDeleted(uuid);
		}

		void SimpleDatabase::announceContactChanged(openmittsu::protocol::ContactId const& contact) {
			emit contactChanged(contact);
		}

		void SimpleDatabase::announceGroupChanged(openmittsu::protocol::GroupId const& group) {
			emit groupChanged(group);
		}

		void SimpleDatabase::announceNewMessage(openmittsu::protocol::ContactId const& contact, QString const& messageUuid) {
			emit contactHasNewMessage(contact, messageUuid);
		}

		void SimpleDatabase::announceNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) {
			emit groupHasNewMessage(group, messageUuid);
		}

		void SimpleDatabase::announceReceivedNewMessage(openmittsu::protocol::ContactId const& contact) {
			emit receivedNewContactMessage(contact);
		}

		void SimpleDatabase::announceReceivedNewMessage(openmittsu::protocol::GroupId const& group) {
			emit receivedNewGroupMessage(group);
		}

		void SimpleDatabase::sendAllWaitingMessages(std::shared_ptr<openmittsu::dataproviders::SentMessageAcceptor> messageAcceptor) {
			{
				QSqlQuery query(database);
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `uid`, `created_at`, `contact_message_type`, `body`, `caption` FROM `contact_messages` WHERE `is_outbox` = 1 AND `is_queued` = 0 AND `is_sent` = 0;"));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute message enumeration query for table contact_messages. Query error: " << query.lastError().text().toStdString();
				}

				while (query.next()) {
					ContactMessageType const messageType = ContactMessageTypeHelper::fromString(query.value(QStringLiteral("contact_message_type")).toString());
					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					QString const uuid(query.value(QStringLiteral("uid")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					internal::DatabaseContactMessage message(this, receiver, messageId);

					switch (messageType) {
						case ContactMessageType::AUDIO:
						{
							MediaFileItem const mediaFile = message.getContentAsMediaFile();
							if (mediaFile.isAvailable()) {
								QString const text = message.getContentAsText();
								QRegularExpression regex(R"(\[(\d+),(?:true|false),"(?:[^"]|\\")*","(?:[^"]|\\")*"\])");
								QRegularExpressionMatchIterator i = regex.globalMatch(text);

								quint16 lengthInSeconds = 1;
								if (i.hasNext()) {
									QRegularExpressionMatch match = i.next();
									lengthInSeconds = match.captured(1).toUShort();
								} else {
									LOGGER()->warn("Failed to extract length of audio clip from database field text: {}", text.toStdString());
								}

								messageAcceptor->processSentContactMessageAudio(receiver, messageId, message.getCreatedAt(), mediaFile.getData(), lengthInSeconds);
								message.setIsQueued(true);
							}
							break;
						}
						case ContactMessageType::IMAGE:
						{
							MediaFileItem const mediaFile = message.getContentAsMediaFile();
							if (mediaFile.isAvailable()) {
								messageAcceptor->processSentContactMessageImage(receiver, messageId, message.getCreatedAt(), mediaFile.getData(), message.getCaption());
								message.setIsQueued(true);
							}
							break;
						}
						case ContactMessageType::LOCATION:
							messageAcceptor->processSentContactMessageLocation(receiver, messageId, message.getCreatedAt(), message.getContentAsLocation());
							message.setIsQueued(true);
							break;
						case ContactMessageType::POLL:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type POLL?!";
							break;
						case ContactMessageType::TEXT:
							messageAcceptor->processSentContactMessageText(receiver, messageId, message.getCreatedAt(), message.getContentAsText());
							message.setIsQueued(true);
							break;
						case ContactMessageType::VIDEO:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type VIDEO?!";
							break;
						default:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting contact message has unhandled type " << query.value(QStringLiteral("type")).toString().toStdString() << "?!";
					}
				}
			}
			{
				QSqlQuery query(database);
				query.prepare(QStringLiteral("SELECT `group_id`, `group_creator`, `apiid`, `uid`, `created_at`, `group_message_type`, `body`, `caption` FROM `group_messages` WHERE `is_outbox` = 1 AND `is_queued` = 0 AND `is_sent` = 0;"));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute message enumeration query for table group_messages. Query error: " << query.lastError().text().toStdString();
				}

				while (query.next()) {
					GroupMessageType const messageType = GroupMessageTypeHelper::fromString(query.value(QStringLiteral("group_message_type")).toString());
					openmittsu::protocol::GroupId const group(openmittsu::protocol::ContactId(query.value(QStringLiteral("group_creator")).toString()), query.value(QStringLiteral("group_id")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					internal::DatabaseGroupMessage message(this, group, messageId);

					switch (messageType) {
						case GroupMessageType::AUDIO:
						{
							MediaFileItem const mediaFile = message.getContentAsMediaFile();
							if (mediaFile.isAvailable()) {
								QString const text = message.getContentAsText();
								QRegularExpression regex(R"(\[(\d+),(?:true|false),"(?:[^"]|\\")*","(?:[^"]|\\")*"\])");
								QRegularExpressionMatchIterator i = regex.globalMatch(text);

								quint16 lengthInSeconds = 1;
								if (i.hasNext()) {
									QRegularExpressionMatch match = i.next();
									lengthInSeconds = match.captured(1).toUShort();
								} else {
									LOGGER()->warn("Failed to extract length of audio clip from database field text: {}", text.toStdString());
								}

								messageAcceptor->processSentGroupMessageAudio(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), mediaFile.getData(), lengthInSeconds);
								message.setIsQueued(true);
							}
							break;
						}
						case GroupMessageType::IMAGE:
						{
							MediaFileItem const mediaFile = message.getContentAsMediaFile();
							if (mediaFile.isAvailable()) {
								messageAcceptor->processSentGroupMessageImage(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), mediaFile.getData(), message.getCaption());
								message.setIsQueued(true);
							}
							break;
						}
						case GroupMessageType::LOCATION:
							messageAcceptor->processSentGroupMessageLocation(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), message.getContentAsLocation());
							message.setIsQueued(true);
							break;
						case GroupMessageType::POLL:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type POLL?!";
							break;
						case GroupMessageType::TEXT:
							messageAcceptor->processSentGroupMessageText(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), message.getContentAsText());
							message.setIsQueued(true);
							break;
						case GroupMessageType::VIDEO:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type VIDEO?!";
							break;
						case GroupMessageType::SYNC_REQUEST:
							messageAcceptor->processSentGroupSyncRequest(group, { group.getOwner() }, messageId, message.getCreatedAt());
							message.setIsQueued(true);
							break;
						case GroupMessageType::SET_IMAGE:
						{
							MediaFileItem const image = m_contactAndGroupDataProvider.getGroupImage(group);
							if (image.isAvailable()) {
								messageAcceptor->processSentGroupSetImage(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), image.getData());
								message.setIsQueued(true);
								break;
							}
						}
						case GroupMessageType::SET_TITLE:
						{
							GroupData groupData = m_contactAndGroupDataProvider.getGroupData(group, false);
							messageAcceptor->processSentGroupSetTitle(group, groupData.members, messageId, message.getCreatedAt(), groupData.title);
							message.setIsQueued(true);
							break;
						}
						case GroupMessageType::GROUP_CREATION:
						{
							auto const members = m_contactAndGroupDataProvider.getGroupMembers(group, false);
							messageAcceptor->processSentGroupCreation(group, members, messageId, message.getCreatedAt(), members);
							message.setIsQueued(true);
							break;
						}
						case GroupMessageType::LEAVE:
							messageAcceptor->processSentGroupLeave(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), m_selfContact);
							message.setIsQueued(true);
							break;
						default:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting group message has unhandled type " << query.value(QStringLiteral("type")).toString().toStdString() << "?!";
					}
				}
			}
			{
				QSqlQuery query(database);
				query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `related_message_apiid`, `uid`, `created_at`, `control_message_type` FROM `control_messages` WHERE `is_outbox` = 1 AND `is_queued` = 0 AND `is_sent` = 0;"));
				if (!query.exec() || !query.isSelect()) {
					throw openmittsu::exceptions::InternalErrorException() << "Could not execute message enumeration query for table control_messages. Query error: " << query.lastError().text().toStdString();
				}

				while (query.next()) {
					ControlMessageType const messageType = ControlMessageTypeHelper::fromString(query.value(QStringLiteral("control_message_type")).toString());
					openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
					openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
					openmittsu::protocol::MessageId const relatedMessageId(query.value(QStringLiteral("related_message_apiid")).toString());
					internal::DatabaseControlMessage message(this, receiver, messageId, relatedMessageId, messageType);

					switch (messageType) {
						case ControlMessageType::AGREE:
							messageAcceptor->processSentContactMessageReceiptAgree(receiver, messageId, message.getCreatedAt(), relatedMessageId);
							message.setIsQueued(true);
							break;
						case ControlMessageType::DISAGREE:
							messageAcceptor->processSentContactMessageReceiptDisagree(receiver, messageId, message.getCreatedAt(), relatedMessageId);
							message.setIsQueued(true);
							break;
						case ControlMessageType::READ:
							messageAcceptor->processSentContactMessageReceiptSeen(receiver, messageId, message.getCreatedAt(), relatedMessageId);
							message.setIsQueued(true);
							break;
						case ControlMessageType::RECEIVED:
							messageAcceptor->processSentContactMessageReceiptReceived(receiver, messageId, message.getCreatedAt(), relatedMessageId);
							message.setIsQueued(true);
							break;
						default:
							throw openmittsu::exceptions::InternalErrorException() << "A waiting control message has unhandled type " << query.value(QStringLiteral("control_type")).toString().toStdString() << "?!";
					}
				}
			}
		}

		QSqlQuery SimpleDatabase::getQueryObject() const {
			return QSqlQuery(database);
		}

		bool SimpleDatabase::transactionStart() {
			return database.transaction();
		}
		
		bool SimpleDatabase::transactionCommit() {
			return database.commit();
		}

		std::shared_ptr<DatabaseReadonlyContactMessage> SimpleDatabase::getContactMessage(openmittsu::protocol::ContactId const& contact, QString const& uuid) {
			internal::DatabaseContactMessageCursor cursor(this, contact, uuid);
			return cursor.getReadonlyMessage();
		}

		std::shared_ptr<DatabaseReadonlyGroupMessage> SimpleDatabase::getGroupMessage(openmittsu::protocol::GroupId const& group, QString const& uuid) {
			internal::DatabaseGroupMessageCursor cursor(this, group, uuid);
			return cursor.getReadonlyMessage();
		}

		ContactData SimpleDatabase::getContactData(openmittsu::protocol::ContactId const& contact, bool fetchMessageCount) const {
			return m_contactAndGroupDataProvider.getContactData(contact, fetchMessageCount);
		}

		ContactToContactDataMap SimpleDatabase::getContactDataAll(bool fetchMessageCount) const {
			return m_contactAndGroupDataProvider.getContactDataAll(fetchMessageCount);
		}
		
		GroupData SimpleDatabase::getGroupData(openmittsu::protocol::GroupId const& group, bool withDescription) const {
			return m_contactAndGroupDataProvider.getGroupData(group, withDescription);
		}
		
		GroupToGroupDataMap SimpleDatabase::getGroupDataAll(bool withDescription) const {
			return m_contactAndGroupDataProvider.getGroupDataAll(withDescription);
		}

		QVector<QString> SimpleDatabase::getLastMessageUuids(openmittsu::protocol::ContactId const& contact, std::size_t n) {
			internal::DatabaseContactMessageCursor cursor(this, contact);
			return cursor.getLastMessages(n);
		}

		QVector<QString> SimpleDatabase::getLastMessageUuids(openmittsu::protocol::GroupId const& group, std::size_t n) {
			internal::DatabaseGroupMessageCursor cursor(this, group);
			return cursor.getLastMessages(n);
		}

	}
}
