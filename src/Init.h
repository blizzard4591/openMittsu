#ifndef OPENMITTSU_INIT_H_
#define OPENMITTSU_INIT_H_

#include <cstdint>
#include <iostream>
#include <vector>

#include <QByteArray>
#include <QFile>
#include <QFontDatabase>
#include <QMessageBox>
#include <QStringList>

#include "sodium.h"

#include "src/utility/Logging.h"
#include "Config.h"

// For Type registration
#include "src/crypto/PublicKey.h"
#include "src/protocol/ContactId.h"
#include <QSet>
#include "src/protocol/GroupId.h"
#include "src/protocol/MessageId.h"
#include "src/protocol/MessageTime.h"
#include "src/messages/FullMessageHeader.h"
#include <QSharedPointer>
#include "src/messages/contact/PreliminaryContactMessage.h"
#include "src/messages/group/PreliminaryGroupMessage.h"
#include "src/tasks/CallbackTask.h"

#define OPENMITTSU_REGISTER_TYPES() do { \
	qRegisterMetaType<openmittsu::crypto::PublicKey>(); \
	qRegisterMetaType<openmittsu::protocol::ContactId>(); \
	qRegisterMetaType<QSet<openmittsu::protocol::ContactId>>(); \
	qRegisterMetaType<openmittsu::protocol::GroupId>(); \
	qRegisterMetaType<openmittsu::protocol::MessageId>(); \
	qRegisterMetaType<openmittsu::protocol::MessageTime>(); \
	qRegisterMetaType<openmittsu::messages::FullMessageHeader>(); \
	qRegisterMetaType<QSharedPointer<openmittsu::messages::FullMessageHeader const>>(); \
	qRegisterMetaType<openmittsu::messages::contact::PreliminaryContactMessage>(); \
	qRegisterMetaType<openmittsu::messages::group::PreliminaryGroupMessage>(); \
} while (false)

bool initializeLogging(std::size_t maxLogfileSize, std::size_t maxFileCount) {
	try {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("openMittsu.log", maxLogfileSize, maxFileCount));
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
			QMessageBox::critical(nullptr, "Error while loading fonts", "A critical error occurred while loading and preparing the embedded fonts.\nThe program will now terminate.\nCode 1");
			return false;
		} else {
			QByteArray fontData(res.readAll());
			fontId = QFontDatabase::addApplicationFontFromData(fontData);
			if (fontId == -1) {
				LOGGER()->critical("Failed to load font \"{}\": addApplicationFontFromData returned -1.", (*it).toStdString());
				QMessageBox::critical(nullptr, "Error while loading fonts", "A critical error occurred while loading and preparing the embedded fonts.\nThe program will now terminate.\nCode 2");
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
		QMessageBox::critical(nullptr, "Error while initializing LibSodium", "A critical error occurred while loading and initializing LibSodium.\nThe program will now terminate.\n");
		return false;
	}

	return true;
}

#endif // OPENMITTSU_INIT_H_
