#include "src/database/Database.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <QUuid>
#include <QSet>

#include <iostream>
#include "src/crypto/Crc32.h"
#include "src/exceptions/InternalErrorException.h"
#include "src/utility/Logging.h"
#include "src/utility/MakeUnique.h"
#include "src/utility/QObjectConnectionMacro.h"


namespace openmittsu {
	namespace database {

		using namespace openmittsu::dataproviders::messages;

Database::Database(QString const& filename, QString const& password, QDir const& mediaStorageLocation) : MessageStorage(), database(), m_driverNameCrypto("QSQLCIPHER"), m_driverNameStandard("QSQLITE"), m_connectionName("openMittsuDatabaseConnection"), m_password(password), m_selfContact(0), m_selfLongTermKeyPair(), m_identityBackup(), m_contactAndGroupDataProvider(*this), m_mediaFileStorage(mediaStorageLocation, *this) {
	if (!(QSqlDatabase::isDriverAvailable(m_driverNameCrypto) || QSqlDatabase::isDriverAvailable(m_driverNameStandard))) {
		throw openmittsu::exceptions::InternalErrorException() << "Neither the SQL driver " << m_driverNameCrypto.toStdString() << " nor the driver " << m_driverNameStandard.toStdString() << " are available. Available are: " << QSqlDatabase::drivers().join(", ").toStdString();
	}

	if (QSqlDatabase::isDriverAvailable(m_driverNameCrypto)) {
		LOGGER()->info("Using the crypto-database interface (QSQLCIPHER).");
		database = QSqlDatabase::addDatabase(m_driverNameCrypto, m_connectionName);
		m_usingCryptoDb = true;
	} else {
		LOGGER()->info("Using the non-crypto-database interface (QSQLITE).");
		database = QSqlDatabase::addDatabase(m_driverNameStandard, m_connectionName);
		m_usingCryptoDb = false;
	}
	database.setDatabaseName(filename);
	if (!database.open()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file.";
	} else if (database.isOpenError()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, error: " << database.lastError().text().toStdString();
	} else if (!database.isOpen()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, open failed.";
	}

	setKey(m_password);

	QStringList const tables = database.tables(QSql::AllTables);
	if (!tables.contains(QStringLiteral("sqlite_master"))) {
		throw openmittsu::exceptions::InternalErrorException() << "SQLITE master table does not exist, invalid database.";
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

Database::Database(QString const& filename, openmittsu::protocol::ContactId const& selfContact, openmittsu::crypto::KeyPair const& selfLongTermKeyPair, QString const& password, QDir const& mediaStorageLocation) : MessageStorage(), database(), m_driverNameCrypto("QSQLCIPHER"), m_driverNameStandard("QSQLITE"), m_connectionName("openMittsuDatabaseConnection"), m_password(password), m_selfContact(selfContact), m_selfLongTermKeyPair(selfLongTermKeyPair), m_identityBackup(std::make_unique<openmittsu::backup::IdentityBackup>(selfContact, selfLongTermKeyPair)), m_contactAndGroupDataProvider(*this), m_mediaFileStorage(mediaStorageLocation, *this) {
	if (!(QSqlDatabase::isDriverAvailable(m_driverNameCrypto) || QSqlDatabase::isDriverAvailable(m_driverNameStandard))) {
		throw openmittsu::exceptions::InternalErrorException() << "Neither the SQL driver " << m_driverNameCrypto.toStdString() << " nor the driver " << m_driverNameStandard.toStdString() << " are available. Available are: " << QSqlDatabase::drivers().join(", ").toStdString();
	}

	if (QSqlDatabase::isDriverAvailable(m_driverNameCrypto)) {
		LOGGER()->info("Using the crypto-database interface (QSQLCIPHER).");
		database = QSqlDatabase::addDatabase(m_driverNameCrypto, m_connectionName);
		m_usingCryptoDb = true;
	} else {
		LOGGER()->info("Using the non-crypto-database interface (QSQLITE).");
		database = QSqlDatabase::addDatabase(m_driverNameStandard, m_connectionName);
		m_usingCryptoDb = false;
	}
	database.setDatabaseName(filename);
	if (!database.open()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file.";
	} else if (database.isOpenError()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, error: " << database.lastError().text().toStdString();
	} else if (!database.isOpen()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not open database file, open failed.";
	}

	setKey(m_password);

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

Database::~Database() {
	if (database.isOpen()) {
		database.close();
		database.removeDatabase(m_connectionName);
	}
}

void Database::setKey(QString const& password) {
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

void Database::enableTimers() {
	queueTimeoutTimer.start();

	QTimer::singleShot(500, this, SLOT(onQueueTimeoutTimerFire()));
}

void Database::setupQueueTimer() {
	OPENMITTSU_CONNECT_QUEUED(&queueTimeoutTimer, timeout(), this, onQueueTimeoutTimerFire());
	queueTimeoutTimer.setInterval(30 * 1000);
}

void Database::onQueueTimeoutTimerFire() {
	LOGGER_DEBUG("Database queue timeout timer fired, checking database...");

	bool resetQueueChangedAnItem = false;
	resetQueueChangedAnItem = resetQueueChangedAnItem || DatabaseContactMessage::resetQueueStatus(*this, 30);
	resetQueueChangedAnItem = resetQueueChangedAnItem || DatabaseGroupMessage::resetQueueStatus(*this, 30);
	resetQueueChangedAnItem = resetQueueChangedAnItem || DatabaseControlMessage::resetQueueStatus(*this, 30);

	if (resetQueueChangedAnItem) {
		emit haveQueuedMessages();
	}
}

QString Database::getDefaultDatabaseFileName() {
	return QStringLiteral("openmittsu.sqlite");
}

QString Database::getCreateStatementForTable(Tables const& table) {
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

QString Database::getTableName(Tables const& table) {
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

bool Database::doesTableExist(Tables const& table) {
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

void Database::setTableVersion(Tables const& table, int tableVersion) {
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

int Database::getTableVersion(Tables const& table) {
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

int Database::createTableIfMissingAndGetVersion(Tables const& table, int createStatementVersion) {
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

void Database::createOrUpdateTables() {
	int versionTableVersions = createTableIfMissingAndGetVersion(Tables::TableVersions, 1);
	int versionTableContacts = createTableIfMissingAndGetVersion(Tables::Contacts, 1);
	int versionTableContactMessages = createTableIfMissingAndGetVersion(Tables::ContactMessages, 1);
	int versionTableControlMessages = createTableIfMissingAndGetVersion(Tables::ControlMessages, 1);
	int versionTableFeatureLevels = createTableIfMissingAndGetVersion(Tables::FeatureLevels, 1);
	int versionTableGroups = createTableIfMissingAndGetVersion(Tables::Groups, 1);
	int versionTableGroupMessages = createTableIfMissingAndGetVersion(Tables::GroupMessages, 1);
	int versionTableMedia = createTableIfMissingAndGetVersion(Tables::Media, 1);
	int versionTableSettings = createTableIfMissingAndGetVersion(Tables::Settings, 1);

	// No updates neccessary as of yet.
}

QString Database::generateUuid() const {
	QUuid const uuid = QUuid::createUuid();
	
	QString const uuidString(uuid.toString());
	if (uuidString.startsWith('{')) {
		return uuidString.mid(1, uuidString.length() - 2);
	}

	return uuidString;
}

openmittsu::protocol::MessageId Database::getNextMessageId(openmittsu::protocol::ContactId const& contact) {
	openmittsu::protocol::MessageId id(openmittsu::protocol::MessageId::random());

	while (true) {
		if (!DatabaseContactMessage::exists(*this, contact, id) && !DatabaseControlMessage::exists(*this, contact, id)) {
			break;
		}

		id = openmittsu::protocol::MessageId::random();
	}

	return id;
}

openmittsu::protocol::MessageId Database::getNextMessageId(openmittsu::protocol::GroupId const& group) {
	openmittsu::protocol::MessageId id(openmittsu::protocol::MessageId::random());

	while (true) {
		if (!DatabaseGroupMessage::exists(*this, group, id)) {
			break;
		}

		id = openmittsu::protocol::MessageId::random();
	}

	return id;
}

openmittsu::protocol::MessageId Database::storeSentContactMessageText(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
	return DatabaseContactMessage::insertContactMessageFromUs(*this, receiver, generateUuid(), timeCreated, ContactMessageType::TEXT, message, isQueued, false, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentContactMessageImage(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
	if (!hasContact(receiver)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not save contact image message, the given contact " << receiver.toString() << " is unknown!";
	}

	QString const uuid = insertMediaItem(image);
	return DatabaseContactMessage::insertContactMessageFromUs(*this, receiver, uuid, timeCreated, ContactMessageType::IMAGE, QStringLiteral(""), isQueued, false, caption);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageLocation(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
	return DatabaseContactMessage::insertContactMessageFromUs(*this, receiver, generateUuid(), timeCreated, ContactMessageType::LOCATION, location.toDatabaseString(), isQueued, false, location.getDescription());
}

openmittsu::protocol::MessageId Database::storeSentGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& message) {
	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::TEXT, message, isQueued, false, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, QString const& caption) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not save group image message, the given group " << group.toString() << " is unknown!";
	}

	QString const uuid = insertMediaItem(image);
	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, uuid, timeCreated, GroupMessageType::IMAGE, QStringLiteral(""), isQueued, false, caption);
}

openmittsu::protocol::MessageId Database::storeSentGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::utility::Location const& location) {
	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::LOCATION, location.toDatabaseString(), isQueued, false, location.getDescription());
}

openmittsu::protocol::MessageId Database::storeSentContactMessageReceiptReceived(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
	return DatabaseControlMessage::insertControlMessageFromUs(*this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::RECEIVED);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageReceiptSeen(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
	return DatabaseControlMessage::insertControlMessageFromUs(*this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::READ);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageReceiptAgree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
	return DatabaseControlMessage::insertControlMessageFromUs(*this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::AGREE);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, openmittsu::protocol::MessageId const& referredMessage) {
	return DatabaseControlMessage::insertControlMessageFromUs(*this, receiver, referredMessage, ControlMessageState::SENDING, timeCreated, isQueued, ControlMessageType::DISAGREE);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageNotificationTypingStarted(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
	// not stored
	return getNextMessageId(receiver);
}

openmittsu::protocol::MessageId Database::storeSentContactMessageNotificationTypingStopped(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
	// not stored
	return getNextMessageId(receiver);
}

void Database::storeReceivedContactMessageText(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
	DatabaseContactMessage::insertContactMessageFromThem(*this, sender, messageId, generateUuid(), timeSent, timeReceived, ContactMessageType::TEXT, message, false, QStringLiteral(""));
}

void Database::storeReceivedContactMessageImage(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
	QString const uuid = insertMediaItem(image);
	DatabaseContactMessage::insertContactMessageFromThem(*this, sender, messageId, uuid, timeSent, timeReceived, ContactMessageType::IMAGE, QStringLiteral(""), false, caption);
}

void Database::storeReceivedContactMessageLocation(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
	DatabaseContactMessage::insertContactMessageFromThem(*this, sender, messageId, generateUuid(), timeSent, timeReceived, ContactMessageType::LOCATION, location.toDatabaseString(), false, location.getDescription());
}

void Database::storeReceivedContactMessageReceiptReceived(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
	try {
		DatabaseContactMessage message(*this, sender, referredMessageId);
		message.setMessageState(UserMessageState::DELIVERED, timeSent);
	} catch (openmittsu::exceptions::InternalErrorException& iee) {
		LOGGER()->warn("Could not saved received \"received\" receipt: {}", iee.what());
	}
}

void Database::storeReceivedContactMessageReceiptSeen(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
	try {
		DatabaseContactMessage message(*this, sender, referredMessageId);
		message.setMessageState(UserMessageState::READ, timeSent);
	} catch (openmittsu::exceptions::InternalErrorException& iee) {
		LOGGER()->warn("Could not saved received \"seen\" receipt: {}", iee.what());
	}
}

void Database::storeReceivedContactMessageReceiptAgree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
	try {
		DatabaseContactMessage message(*this, sender, referredMessageId);
		message.setMessageState(UserMessageState::USERACK, timeSent);
	} catch (openmittsu::exceptions::InternalErrorException& iee) {
		LOGGER()->warn("Could not saved received \"agree\" receipt: {}", iee.what());
	}
}

void Database::storeReceivedContactMessageReceiptDisagree(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageId const& referredMessageId) {
	try {
		DatabaseContactMessage message(*this, sender, referredMessageId);
		message.setMessageState(UserMessageState::USERDEC, timeSent);
	} catch (openmittsu::exceptions::InternalErrorException& iee) {
		LOGGER()->warn("Could not saved received \"disagree\" receipt: {}", iee.what());
	}
}

void Database::storeReceivedContactTypingNotificationTyping(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
	if (!hasContact(sender)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not save contact typing notfication, the given contact " << sender.toString() << " is unknown!";
	}
	
	emit contactStartedTyping(sender);
}

void Database::storeReceivedContactTypingNotificationStopped(openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent) {
	if (!hasContact(sender)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not save contact typing notfication, the given contact " << sender.toString() << " is unknown!";
	}
	
	emit contactStoppedTyping(sender);
}

void Database::storeReceivedGroupMessageText(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& message) {
	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::TEXT, message, false, QStringLiteral(""));
}

void Database::storeReceivedGroupMessageImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image, QString const& caption) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not save group image message, the given group " << group.toString() << " is unknown!";
	}

	QString const uuid = insertMediaItem(image);
	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::IMAGE, QStringLiteral(""), false, caption);
}

void Database::storeReceivedGroupMessageLocation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, openmittsu::utility::Location const& location) {
	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::LOCATION, location.toDatabaseString(), false, location.getDescription());
}

void Database::storeReceivedGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group leave request!";
	}

	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::LEAVE, QStringLiteral(""), true, QStringLiteral(""));

	QSet<openmittsu::protocol::ContactId> currentMembers = m_contactAndGroupDataProvider.getGroupMembers(group, false);
	currentMembers.remove(sender);
	m_contactAndGroupDataProvider.setGroupMembers(group, currentMembers);
}

void Database::storeReceivedGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group sync request!";
	} else if (!(group.getOwner() == getSelfContact())) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not store group sync request from " << sender.toString() << "!";
	}

	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::SYNC_REQUEST, QStringLiteral(""), true, QStringLiteral(""));
}

void Database::storeReceivedGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QString const& title) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group title!";
	} else if (!(group.getOwner() == sender)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by " << sender.toString() << ", can not set group title!";
	} else {
		DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::SET_TITLE, title, true, QStringLiteral(""));
		m_contactAndGroupDataProvider.setGroupTitle(group, title);
	}
}

void Database::storeReceivedGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QByteArray const& image) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group image!";
	} else if (!(group.getOwner() == sender)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by " << sender.toString() << ", can not set group image!";
	} else {
		QString const uuid = insertMediaItem(image);
		DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, uuid, timeSent, timeReceived, GroupMessageType::SET_IMAGE, QStringLiteral(""), true, QStringLiteral(""));
		m_contactAndGroupDataProvider.setGroupImage(group, image);
	}
}

void Database::storeReceivedGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::ContactId const& sender, openmittsu::protocol::MessageId const& messageId, openmittsu::protocol::MessageTime const& timeSent, openmittsu::protocol::MessageTime const& timeReceived, QSet<openmittsu::protocol::ContactId> const& members) {
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

	DatabaseGroupMessage::insertGroupMessageFromThem(*this, group, sender, messageId, generateUuid(), timeSent, timeReceived, GroupMessageType::GROUP_CREATION, result, true, QStringLiteral(""));
	
	m_contactAndGroupDataProvider.addGroup(group, "", timeSent, members, false, false);
}

void Database::storeMessageSendFailed(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
	if (DatabaseControlMessage::exists(*this, receiver, messageId)) {
		DatabaseControlMessage message(DatabaseControlMessage::fromReceiverAndControlMessageId(*this, receiver, messageId));
		message.setMessageState(ControlMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
	} else {
		DatabaseContactMessage message(*this, receiver, messageId);
		message.setMessageState(UserMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
	}
}

void Database::storeMessageSendDone(openmittsu::protocol::ContactId const& receiver, openmittsu::protocol::MessageId const& messageId) {
	if (DatabaseControlMessage::exists(*this, receiver, messageId)) {
		DatabaseControlMessage message(DatabaseControlMessage::fromReceiverAndControlMessageId(*this, receiver, messageId));
		message.setMessageState(ControlMessageState::SENT, openmittsu::protocol::MessageTime::now());
	} else {
		DatabaseContactMessage message(*this, receiver, messageId);
		message.setMessageState(UserMessageState::SENT, openmittsu::protocol::MessageTime::now());
	}
}

void Database::storeMessageSendFailed(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
	DatabaseGroupMessage message(*this, group, messageId);
	message.setMessageState(UserMessageState::SENDFAILED, openmittsu::protocol::MessageTime::now());
}

void Database::storeMessageSendDone(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageId const& messageId) {
	DatabaseGroupMessage message(*this, group, messageId);
	message.setMessageState(UserMessageState::SENT, openmittsu::protocol::MessageTime::now());
}

void Database::storeContactMessagesFromBackup(QList<openmittsu::backup::ContactMessageBackupObject> const& messages) {
	DatabaseContactMessage::insertContactMessagesFromBackup(*this, messages);
}

void Database::storeGroupMessagesFromBackup(QList<openmittsu::backup::GroupMessageBackupObject> const& messages) {
	DatabaseGroupMessage::insertGroupMessagesFromBackup(*this, messages);
}

void Database::storeContactMediaItemsFromBackup(QList<openmittsu::backup::ContactMediaItemBackupObject> const& items) {
	m_mediaFileStorage.insertMediaItemsFromBackup(items);
}

void Database::storeGroupMediaItemsFromBackup(QList<openmittsu::backup::GroupMediaItemBackupObject> const& items) {
	m_mediaFileStorage.insertMediaItemsFromBackup(items);
}

void Database::storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus, QString const& firstName, QString const& lastName, QString const& nickName, int color) {
	m_contactAndGroupDataProvider.addContact(contact, publicKey, verificationStatus, firstName, lastName, nickName, color);
}

void Database::storeNewContact(openmittsu::protocol::ContactId const& contact, openmittsu::crypto::PublicKey const& publicKey) {
	storeNewContact(contact, publicKey, openmittsu::protocol::ContactIdVerificationStatus::VERIFICATION_STATUS_UNVERIFIED, QStringLiteral(""), QStringLiteral(""), QStringLiteral(""), 0);
}

void Database::storeNewGroup(openmittsu::protocol::GroupId const& groupId, QString const& name, openmittsu::protocol::MessageTime const& createdAt, QSet<openmittsu::protocol::ContactId> const& members, bool isDeleted, bool isAwaitingSync) {
	for (openmittsu::protocol::ContactId const& c : members) {
		if (!hasContact(c)) {
			throw openmittsu::exceptions::InternalErrorException() << "The given group "<< groupId.toString() << " contains member " << c.toString() << " that is unknown.";
		}
	}

	m_contactAndGroupDataProvider.addGroup(groupId, name, createdAt, members, isDeleted, isAwaitingSync);
}

void Database::storeNewGroup(openmittsu::protocol::GroupId const& groupId, QSet<openmittsu::protocol::ContactId> const& members, bool isAwaitingSync) {
	storeNewGroup(groupId, "", openmittsu::protocol::MessageTime::now(), members, false, isAwaitingSync);
}

openmittsu::protocol::MessageId Database::storeSentGroupCreation(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QSet<openmittsu::protocol::ContactId> const& members, bool apply) {
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
		m_contactAndGroupDataProvider.addGroup(group, "", timeCreated, members, false, false);
	}

	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::GROUP_CREATION, memberString, isQueued, true, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentGroupSetTitle(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QString const& groupTitle, bool apply) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group title!";
	} else if (!(group.getOwner() == getSelfContact())) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not set group title!";
	}
	
	if (apply) {
		m_contactAndGroupDataProvider.setGroupTitle(group, groupTitle);
	}

	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::SET_TITLE, groupTitle, isQueued, true, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentGroupSetImage(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, QByteArray const& image, bool apply) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not set group image!";
	} else if (!(group.getOwner() == getSelfContact())) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not owned by us, can not set group image!";
	}

	if (apply) {
		m_contactAndGroupDataProvider.setGroupImage(group, image);
	}

	QString const uuid = insertMediaItem(image);
	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, uuid, timeCreated, GroupMessageType::SET_IMAGE, QStringLiteral(""), isQueued, true, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentGroupSyncRequest(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued) {
	if (group.getOwner() == getSelfContact()) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by us, can not store group sync request from us!";
	}
	
	return DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::SYNC_REQUEST, QStringLiteral(""), isQueued, true, QStringLiteral(""));
}

openmittsu::protocol::MessageId Database::storeSentGroupLeave(openmittsu::protocol::GroupId const& group, openmittsu::protocol::MessageTime const& timeCreated, bool isQueued, bool apply) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is not known, can not store group leave request!";
	} else if (group.getOwner() == getSelfContact()) {
		throw openmittsu::exceptions::InternalErrorException() << "The given group " << group.toString() << " is owned by us, can not store group leave request from us!";
	}

	openmittsu::protocol::MessageId const messageId = DatabaseGroupMessage::insertGroupMessageFromUs(*this, group, generateUuid(), timeCreated, GroupMessageType::LEAVE, QStringLiteral(""), isQueued, true, QStringLiteral(""));
	
	if (apply) {
		QSet<openmittsu::protocol::ContactId> currentMembers = m_contactAndGroupDataProvider.getGroupMembers(group, false);
		currentMembers.remove(m_selfContact);
		m_contactAndGroupDataProvider.setGroupMembers(group, currentMembers);
	}

	return messageId;
}

bool Database::hasContact(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.hasContact(identity);
}

bool Database::hasGroup(openmittsu::protocol::GroupId const& group) const {
	return m_contactAndGroupDataProvider.hasGroup(group);
}

bool Database::isDeleteted(openmittsu::protocol::GroupId const& group) const {
	return m_contactAndGroupDataProvider.getGroupStatus(group) == openmittsu::protocol::GroupStatus::DELETED;
}

openmittsu::dataproviders::BackedContact Database::getBackedContact(openmittsu::protocol::ContactId const& contact, openmittsu::dataproviders::MessageCenter& messageCenter) {
	return m_contactAndGroupDataProvider.getContact(contact, messageCenter);
}

openmittsu::dataproviders::BackedGroup Database::getBackedGroup(openmittsu::protocol::GroupId const& group, openmittsu::dataproviders::MessageCenter& messageCenter) {
	return m_contactAndGroupDataProvider.getGroup(group, messageCenter);
}

openmittsu::protocol::GroupStatus Database::getGroupStatus(openmittsu::protocol::GroupId const& group) const {
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

openmittsu::protocol::ContactStatus Database::getContactStatus(openmittsu::protocol::ContactId const& contact) const {
	if (hasContact(contact)) {
		return openmittsu::protocol::ContactStatus::KNOWN;
	} else {
		return openmittsu::protocol::ContactStatus::UNKNOWN;
	}
}

QString Database::getOptionValueInternal(QString const& optionName, bool isInternalOption) {
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

bool Database::hasOptionInternal(QString const& optionName, bool isInternalOption) {
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

void Database::setOptionInternal(QString const& optionName, QString const& optionValue, bool isInternalOption) {
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

bool Database::hasOption(QString const& optionName) {
	return hasOptionInternal(optionName, false);
}

QString Database::getOptionValueAsString(QString const& optionName) {
	return getOptionValueInternal(optionName, false);
}

bool Database::getOptionValueAsBool(QString const& optionName) {
	QString const value = getOptionValueInternal(optionName, false);
	if (value == QStringLiteral("0")) {
		return false;
	} else if (value == QStringLiteral("1")) {
		return true;
	} else {
		throw openmittsu::exceptions::InternalErrorException() << "Could not convert value \"" << value.toStdString() << "\" of option " << optionName.toStdString() << " into a boolean.";
	}
}

QByteArray Database::getOptionValueAsByteArray(QString const& optionName) {
	QString const value = getOptionValueInternal(optionName, false);
	QRegularExpression regexp("^(?:[a-f0-9][a-f0-9])*$");
	QRegularExpressionMatch match = regexp.match(value);
	if (!match.hasMatch()) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not convert value \"" << value.toStdString() << "\" of option " << optionName.toStdString() << " into a boolean.";
	}

	return QByteArray::fromHex(value.toUtf8());
}

void Database::setOptionValue(QString const& optionName, QString const& optionValue) {
	setOptionInternal(optionName, optionValue, false);
}

void Database::setOptionValue(QString const& optionName, bool const& optionValue) {
	if (optionValue) {
		setOptionInternal(optionName, QStringLiteral("1"), false);
	} else {
		setOptionInternal(optionName, QStringLiteral("0"), false);
	}
}

void Database::setOptionValue(QString const& optionName, QByteArray const& optionValue) {
	setOptionInternal(optionName, QString(optionValue.toHex()), false);
}

MediaFileItem Database::getMediaItem(QString const& uuid) const {
	return m_mediaFileStorage.getMediaItem(uuid);
}

QString Database::insertMediaItem(QByteArray const& data) {
	return m_mediaFileStorage.insertMediaItem(data);
}

void Database::removeMediaItem(QString const& uuid) {
	m_mediaFileStorage.removeMediaItem(uuid);
}

openmittsu::backup::IdentityBackup Database::getBackup() {
	return *m_identityBackup;
}

void Database::updateCachedIdentityBackup() {
	QString const identityBackup = getOptionValueInternal(QStringLiteral("identity"), true);
	m_identityBackup = std::make_unique<openmittsu::backup::IdentityBackup>(openmittsu::backup::IdentityBackup::fromBackupString(identityBackup, m_password));
}

openmittsu::protocol::ContactId const& Database::getSelfContact() const {
	return m_selfContact;
}

void Database::setBackup(openmittsu::protocol::ContactId selfId, openmittsu::crypto::KeyPair key) {
	openmittsu::backup::IdentityBackup backupData(selfId, key);
	setOptionInternal(QStringLiteral("identity"), backupData.toBackupString(m_password), true);
}

openmittsu::crypto::PublicKey Database::getContactPublicKey(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.getPublicKey(identity);
}

QString Database::getContactNickname(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.getNickName(identity);
}

openmittsu::protocol::ContactIdVerificationStatus Database::getContactVerficationStatus(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.getVerificationStatus(identity);
}

openmittsu::protocol::AccountStatus Database::getContactAccountStatus(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.getAccountStatus(identity);
}

openmittsu::protocol::FeatureLevel Database::getContactFeatureLevel(openmittsu::protocol::ContactId const& identity) const {
	return m_contactAndGroupDataProvider.getFeatureLevel(identity);
}

int Database::getContactCount() const {
	return m_contactAndGroupDataProvider.getContactCount();
}

int Database::getGroupCount() const {
	return m_contactAndGroupDataProvider.getGroupCount();
}

int Database::getContactMessageCount() const {
	return DatabaseContactMessage::getContactMessageCount(*this);
}

int Database::getGroupMessageCount() const {
	return DatabaseGroupMessage::getGroupMessageCount(*this);
}

int Database::getMediaItemCount() const {
	return m_mediaFileStorage.getMediaItemCount();
}

void Database::setContactNickname(openmittsu::protocol::ContactId const& identity, QString const& nickname) {
	if (!hasContact(identity)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
	} else {
		m_contactAndGroupDataProvider.setNickName(identity, nickname);
	}
}

void Database::setContactAccountStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::AccountStatus const& status) {
	if (!hasContact(identity)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
	} else {
		m_contactAndGroupDataProvider.setAccountStatus(identity, status);
	}
}

void Database::setContactVerficationStatus(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::ContactIdVerificationStatus const& verificationStatus) {
	if (!hasContact(identity)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
	} else {
		m_contactAndGroupDataProvider.setVerificationStatus(identity, verificationStatus);
	}
}

void Database::setContactFeatureLevel(openmittsu::protocol::ContactId const& identity, openmittsu::protocol::FeatureLevel const& featureLevel) {
	if (!hasContact(identity)) {
		throw openmittsu::exceptions::InternalErrorException() << "The given identity " << identity.toString() << " is unknown!";
	} else {
		m_contactAndGroupDataProvider.setFeatureLevel(identity, featureLevel);
	}
}

void Database::setContactAccountStatusBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::AccountStatus> const& status) {
	m_contactAndGroupDataProvider.setAccountStatusBatch(status);
}

void Database::setContactFeatureLevelBatch(QHash<openmittsu::protocol::ContactId, openmittsu::protocol::FeatureLevel> const& featureLevels) {
	m_contactAndGroupDataProvider.setFeatureLevelBatch(featureLevels);
}

QSet<openmittsu::protocol::ContactId> Database::getKnownContacts() const {
	return m_contactAndGroupDataProvider.getKnownContacts();
}

QSet<openmittsu::protocol::ContactId> Database::getContactsRequiringFeatureLevelCheck(int maximalAgeInSeconds) const {
	return m_contactAndGroupDataProvider.getContactsRequiringFeatureLevelCheck(maximalAgeInSeconds);
}

QSet<openmittsu::protocol::ContactId> Database::getContactsRequiringAccountStatusCheck(int maximalAgeInSeconds) const {
	return m_contactAndGroupDataProvider.getContactsRequiringAccountStatusCheck(maximalAgeInSeconds);
}

QSet<openmittsu::protocol::GroupId> Database::getKnownGroups() const {
	return m_contactAndGroupDataProvider.getKnownGroups();
}

QHash<openmittsu::protocol::ContactId, openmittsu::crypto::PublicKey> Database::getKnownContactsWithPublicKeys() const {
	return m_contactAndGroupDataProvider.getKnownContactsWithPublicKeys();
}

QHash<openmittsu::protocol::ContactId, QString> Database::getKnownContactsWithNicknames(bool withSelfContactId) const {
	return m_contactAndGroupDataProvider.getKnownContactsWithNicknames(withSelfContactId);
}

QSet<openmittsu::protocol::ContactId> Database::getGroupMembers(openmittsu::protocol::GroupId const& group, bool excludeSelfContact) const {
	openmittsu::protocol::GroupStatus const status = m_contactAndGroupDataProvider.getGroupStatus(group);
	if ((status != openmittsu::protocol::GroupStatus::KNOWN) && (status != openmittsu::protocol::GroupStatus::TEMPORARY)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not get group members, the given group " << group.toString() << " is unknown!";
	}

	return m_contactAndGroupDataProvider.getGroupMembers(group, excludeSelfContact);
}

QString Database::getGroupTitle(openmittsu::protocol::GroupId const& group) const {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not get group title, the given group " << group.toString() << " is unknown!";
	}

	return m_contactAndGroupDataProvider.getGroupTitle(group);
}

QString Database::getGroupDescription(openmittsu::protocol::GroupId const& group) const {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not get group title, the given group " << group.toString() << " is unknown!";
	}

	return m_contactAndGroupDataProvider.getGroupDescription(group);
}

MediaFileItem Database::getGroupImage(openmittsu::protocol::GroupId const& group) const {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not get group image, the given group " << group.toString() << " is unknown!";
	}

	return m_contactAndGroupDataProvider.getGroupImage(group);
}

bool Database::getGroupIsAwaitingSync(openmittsu::protocol::GroupId const& group) const {
	openmittsu::protocol::GroupStatus const status = m_contactAndGroupDataProvider.getGroupStatus(group);
	return (status == openmittsu::protocol::GroupStatus::TEMPORARY);
}

QHash<openmittsu::protocol::GroupId, std::pair<QSet<openmittsu::protocol::ContactId>, QString>> Database::getKnownGroupsWithMembersAndTitles() const {
	return m_contactAndGroupDataProvider.getKnownGroupsWithMembersAndTitles();
}

QSet<openmittsu::protocol::GroupId> Database::getKnownGroupsContainingMember(openmittsu::protocol::ContactId const& identity) const {
	if (!hasContact(identity)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not get groups containing contact because the given contact " << identity.toString() << " is unknown!";
	}

	return m_contactAndGroupDataProvider.getKnownGroupsContainingMember(identity);
}

DatabaseContactMessageCursor Database::getMessageCursor(openmittsu::protocol::ContactId const& contact) {
	if (!hasContact(contact)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not create message cursor because the given contact " << contact.toString() << " is unknown!";
	}
	return DatabaseContactMessageCursor(*this, contact);
}

DatabaseGroupMessageCursor Database::getMessageCursor(openmittsu::protocol::GroupId const& group) {
	if (!hasGroup(group)) {
		throw openmittsu::exceptions::InternalErrorException() << "Could not create message cursor because the given group " << group.toString() << " is unknown!";
	}
	return DatabaseGroupMessageCursor(*this, group);
}

void Database::announceMessageChanged(QString const& uuid) {
	LOGGER_DEBUG("Database: Announcing messageChanged() for UUID {}.", uuid.toStdString());
	emit messageChanged(uuid);
}

void Database::announceContactChanged(openmittsu::protocol::ContactId const& contact) {
	emit contactChanged(contact);
}

void Database::announceGroupChanged(openmittsu::protocol::GroupId const& group) {
	emit groupChanged(group);
}

void Database::announceNewMessage(openmittsu::protocol::ContactId const& contact, QString const& messageUuid) {
	emit contactHasNewMessage(contact, messageUuid);
}

void Database::announceNewMessage(openmittsu::protocol::GroupId const& group, QString const& messageUuid) {
	emit groupHasNewMessage(group, messageUuid);
}

void Database::announceReceivedNewMessage(openmittsu::protocol::ContactId const& contact) {
	emit receivedNewContactMessage(contact);
}

void Database::announceReceivedNewMessage(openmittsu::protocol::GroupId const& group) {
	emit receivedNewGroupMessage(group);
}

void Database::sendAllWaitingMessages(openmittsu::dataproviders::SentMessageAcceptor& messageAcceptor) {
	{
		QSqlQuery query(database);
		query.prepare(QStringLiteral("SELECT `identity`, `apiid`, `uid`, `created_at`, `contact_message_type`, `body`, `caption` FROM `contact_messages` WHERE `is_outbox` = 1 AND `is_queued` = 0 AND `is_sent` = 0;"));
		if (!query.exec() || !query.isSelect()) {
			throw openmittsu::exceptions::InternalErrorException() << "Could not execute message enumeration query for table contact_messages. Query error: " << query.lastError().text().toStdString();
		} 

		while (query.next()) {
			ContactMessageType const messageType = ContactMessageTypeHelper::fromString(query.value(QStringLiteral("contact_message_type")).toString());
			openmittsu::protocol::ContactId const receiver(query.value(QStringLiteral("identity")).toString());
			openmittsu::protocol::MessageId const messageId(query.value(QStringLiteral("apiid")).toString());
			DatabaseContactMessage message(*this, receiver, messageId);

			switch (messageType) {
				case ContactMessageType::AUDIO:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type AUDIO?!";
					break;
				case ContactMessageType::IMAGE:
				{
					MediaFileItem const image = message.getContentAsImage();
					if (image.isAvailable()) {
						messageAcceptor.processSentContactMessageImage(receiver, messageId, message.getCreatedAt(), image.getData(), message.getCaption());
						message.setIsQueued(true);
					}
					break;
				}
				case ContactMessageType::LOCATION:
					messageAcceptor.processSentContactMessageLocation(receiver, messageId, message.getCreatedAt(), message.getContentAsLocation());
					message.setIsQueued(true);
					break;
				case ContactMessageType::POLL:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type POLL?!";
					break;
				case ContactMessageType::TEXT:
					messageAcceptor.processSentContactMessageText(receiver, messageId, message.getCreatedAt(), message.getContentAsText());
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
			DatabaseGroupMessage message(*this, group, messageId);

			switch (messageType) {
				case GroupMessageType::AUDIO:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type AUDIO?!";
					break;
				case GroupMessageType::IMAGE:
				{
					MediaFileItem const image = message.getContentAsImage();
					if (image.isAvailable()) {
						messageAcceptor.processSentGroupMessageImage(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), image.getData(), message.getCaption());
						message.setIsQueued(true);
						break;
					}
				}
				case GroupMessageType::LOCATION:
					messageAcceptor.processSentGroupMessageLocation(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), message.getContentAsLocation());
					message.setIsQueued(true);
					break;
				case GroupMessageType::POLL:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type POLL?!";
					break;
				case GroupMessageType::TEXT:
					messageAcceptor.processSentGroupMessageText(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), message.getContentAsText());
					message.setIsQueued(true);
					break;
				case GroupMessageType::VIDEO:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting message has type VIDEO?!";
					break;
				case GroupMessageType::SYNC_REQUEST:
					messageAcceptor.processSentGroupSyncRequest(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt());
					message.setIsQueued(true);
					break;
				case GroupMessageType::SET_IMAGE:
				{
					MediaFileItem const image = m_contactAndGroupDataProvider.getGroupImage(group);
					if (image.isAvailable()) {
						messageAcceptor.processSentGroupSetImage(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), image.getData());
						message.setIsQueued(true);
						break;
					}
				}
				case GroupMessageType::SET_TITLE:
					messageAcceptor.processSentGroupSetTitle(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), m_contactAndGroupDataProvider.getGroupTitle(group));
					message.setIsQueued(true);
					break;
				case GroupMessageType::GROUP_CREATION:
					messageAcceptor.processSentGroupCreation(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), m_contactAndGroupDataProvider.getGroupMembers(group, false));
					message.setIsQueued(true);
					break;
				case GroupMessageType::LEAVE:
					messageAcceptor.processSentGroupLeave(group, m_contactAndGroupDataProvider.getGroupMembers(group, false), messageId, message.getCreatedAt(), m_selfContact);
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
			DatabaseControlMessage message(*this, receiver, messageId, relatedMessageId, messageType);

			switch (messageType) {
				case ControlMessageType::AGREE:
					messageAcceptor.processSentContactMessageReceiptAgree(receiver, messageId, message.getCreatedAt(), relatedMessageId);
					message.setIsQueued(true);
					break;
				case ControlMessageType::DISAGREE:
					messageAcceptor.processSentContactMessageReceiptDisagree(receiver, messageId, message.getCreatedAt(), relatedMessageId);
					message.setIsQueued(true);
					break;
				case ControlMessageType::READ:
					messageAcceptor.processSentContactMessageReceiptSeen(receiver, messageId, message.getCreatedAt(), relatedMessageId);
					message.setIsQueued(true);
					break;
				case ControlMessageType::RECEIVED:
					messageAcceptor.processSentContactMessageReceiptReceived(receiver, messageId, message.getCreatedAt(), relatedMessageId);
					message.setIsQueued(true);
					break;
				default:
					throw openmittsu::exceptions::InternalErrorException() << "A waiting control message has unhandled type " << query.value(QStringLiteral("control_type")).toString().toStdString() << "?!";
			}
		}
	}
}

}

}
