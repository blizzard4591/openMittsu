#ifndef OPENMITTSU_INIT_H_
#define OPENMITTSU_INIT_H_

#include <cstdint>
#include <iostream>
#include <vector>

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QStringList>

#ifndef OPENMITTSU_TESTS
#include <QMessageBox>
#endif

#include "sodium.h"

#include "src/utility/Logging.h"
#include "Config.h"

#include <QSet>
#include <QSharedPointer>

// For Type registration
#include "src/crypto/PublicKey.h"
#include "src/database/ContactData.h"
#include "src/database/DatabaseSeekResult.h"
#include "src/database/DatabaseThreadWorker.h"
#include "src/database/DatabaseWrapperFactory.h"
#include "src/database/GroupData.h"
#include "src/database/NewContactData.h"
#include "src/database/NewGroupData.h"
#include "src/messages/FullMessageHeader.h"
#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/messages/contact/ReceiptMessageContent.h"
#include "src/messages/group/PreliminaryGroupMessage.h"
#include "src/options/OptionData.h"
#include "src/options/OptionGroups.h"
#include "src/options/OptionReaderFactory.h"
#include "src/options/OptionStorage.h"
#include "src/options/OptionTypes.h"
#include "src/options/Options.h"
#include "src/protocol/ContactId.h"
#include "src/protocol/ContactIdList.h"
#include "src/protocol/ContactIdWithMessageId.h"
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/utility/Location.h"
#include "src/utility/QExifImageHeader.h"

#define OPENMITTSU_REGISTER_TYPES() do { \
	qRegisterMetaType<openmittsu::crypto::PublicKey>(); \
	qRegisterMetaType<openmittsu::database::ContactData>(); \
	qRegisterMetaType<openmittsu::database::DatabaseSeekResult>(); \
	qRegisterMetaType<openmittsu::database::DatabaseOpenResult>(); \
	qRegisterMetaType<openmittsu::database::DatabaseWrapperFactory>(); \
	qRegisterMetaType<openmittsu::database::GroupData>(); \
	qRegisterMetaType<openmittsu::database::NewContactData>(); \
	qRegisterMetaType<openmittsu::database::NewGroupData>(); \
	qRegisterMetaType<openmittsu::messages::FullMessageHeader>(); \
	qRegisterMetaType<QSharedPointer<openmittsu::messages::FullMessageHeader const>>(); \
	qRegisterMetaType<openmittsu::messages::contact::PreliminaryContactMessage>(); \
	qRegisterMetaType<openmittsu::messages::contact::ReceiptMessageContent::ReceiptType>(); \
	qRegisterMetaType<openmittsu::messages::group::PreliminaryGroupMessage>(); \
	qRegisterMetaType<openmittsu::options::OptionData>(); \
	qRegisterMetaType<openmittsu::options::OptionGroups>(); \
	qRegisterMetaType<openmittsu::options::OptionReaderFactory>(); \
	qRegisterMetaType<openmittsu::options::OptionStorage>(); \
	qRegisterMetaType<openmittsu::options::OptionTypes>(); \
	qRegisterMetaType<openmittsu::options::Options>(); \
	qRegisterMetaType<openmittsu::protocol::ContactId>(); \
	qRegisterMetaType<openmittsu::protocol::ContactIdList>(); \
	qRegisterMetaType<openmittsu::protocol::ContactIdWithMessageId>(); \
	qRegisterMetaType<openmittsu::protocol::GroupId>(); \
	qRegisterMetaType<openmittsu::protocol::MessageId>(); \
	qRegisterMetaType<openmittsu::protocol::MessageTime>(); \
	qRegisterMetaType<openmittsu::utility::Location>(); \
	qRegisterMetaType<QExifURational>(); \
	qRegisterMetaType<QExifSRational>(); \
} while (false)

bool initializeLogging(std::size_t maxLogfileSize, std::size_t maxFileCount) {
	// Get logging directory
#ifndef OPENMITTSU_TESTS
	bool const isTesting = false;
#else
	bool const isTesting = true;
#endif
	QStandardPaths::setTestModeEnabled(isTesting);
	
	
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
	QStandardPaths::StandardLocation const location = QStandardPaths::AppLocalDataLocation;
#else
	QStandardPaths::StandardLocation const location = QStandardPaths::DataLocation;
#endif
	QString const writableLocationString = QStandardPaths::writableLocation(location);
	
	QString logFileName = "openMittsu.log";
	if (writableLocationString.isNull() || writableLocationString.isEmpty()) {
		std::cerr << "Qt StandardPaths returned no writable location, defaulting to local." << std::endl;
	} else {
		QDir folder(writableLocationString);		
		if (!folder.mkpath(writableLocationString)) {
			std::cerr << "Could not create directory for log files \"" << writableLocationString.toStdString() << "\", defaulting to local." << std::endl;
		} else {
			logFileName = folder.absoluteFilePath("openMittsu.log");
		}
	}
	
	QFile logFile(logFileName);
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
		std::cerr << "Error: The log file \"" << logFileName.toStdString() << "\" is not writable!" << std::endl;
		return false;
	}
	logFile.close();	
	std::cout << "Logging location: " << logFileName.toStdString() << std::endl;
	
	try {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName.toStdString(), maxLogfileSize, maxFileCount));
		auto combined_logger = std::make_shared<spdlog::logger>(OPENMITTSU_LOGGING_LOGGER_MAIN_NAME, begin(sinks), end(sinks));

		// Set log level depending on Build Type
		combined_logger->set_level(OPENMITTSU_LOGGING_LEVEL);

		spdlog::register_logger(combined_logger);
	} catch (const spdlog::spdlog_ex& ex) {
		std::cerr << "Setting up the logger failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}

bool initializeFonts() {
	QStringList list;
	list << "SourceSansPro-Regular.ttf" << "DejaVuSans.ttf" << "OpenSansEmoji.ttf";
	int fontId = -1;
	QStringList::const_iterator it = list.constBegin();
	QStringList::const_iterator end = list.constEnd();
	for (; it != end; ++it) {
		QString fileName(":/fonts/");
		fileName.append(*it);
		QFile res(fileName);
		if ((!res.exists()) || (res.open(QFile::ReadOnly) == false)) {
			LOGGER()->critical("Failed to load font \"{}\": file could not be opened for reading.", (*it).toStdString());
#ifndef OPENMITTSU_TESTS
			QMessageBox::critical(nullptr, "Error while loading fonts", "A critical error occurred while loading and preparing the embedded fonts.\nThe program will now terminate.\nCode 1");
#endif
			return false;
		} else {
			QByteArray fontData(res.readAll());
			fontId = QFontDatabase::addApplicationFontFromData(fontData);
			if (fontId == -1) {
				LOGGER()->critical("Failed to load font \"{}\": addApplicationFontFromData returned -1.", (*it).toStdString());
#ifndef OPENMITTSU_TESTS
				QMessageBox::critical(nullptr, "Error while loading fonts", "A critical error occurred while loading and preparing the embedded fonts.\nThe program will now terminate.\nCode 2");
#endif
				return false;
			}
		}
	}
	return true;
}

bool initializeLibSodium() {
	int sodiumInitRet = sodium_init();
	if (sodiumInitRet == -1) {
		LOGGER()->critical("Could not initialize LibSodium, sodium_init() returned -1.");
#ifndef OPENMITTSU_TESTS
		QMessageBox::critical(nullptr, "Error while initializing LibSodium", "A critical error occurred while loading and initializing LibSodium.\nThe program will now terminate.\n");
#endif
		return false;
	}

	return true;
}

#endif // OPENMITTSU_INIT_H_
