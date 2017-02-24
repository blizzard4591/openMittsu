#include <QApplication>
#include <QMessageBox>
#include <QFontDatabase>
#include <QSharedPointer>
#include <QSet>

#include <cstdint>
#include <iostream>
#include <vector>

#include "Client.h"
#include "utility/Version.h"
#include "utility/Logging.h"
#include "utility/OptionMaster.h"
#include "ExceptionHandlingApplication.h"

// For Type registration
#include "PublicKey.h"
#include "protocol/ContactId.h"
#include "protocol/GroupId.h"
#include "protocol/MessageId.h"
#include "protocol/MessageTime.h"
#include "messages/FullMessageHeader.h"
#include "messages/contact/PreliminaryContactMessage.h"
#include "messages/group/PreliminaryGroupMessage.h"

#include "sodium.h"

bool initializeLogging(std::size_t maxLogfileSize, std::size_t maxFileCount) {
	try {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("openMittsu", SPDLOG_FILENAME_T("log"), maxLogfileSize, maxFileCount));
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

int main(int argc, char *argv[]) {
	if (!initializeLogging(OPENMITTSU_LOGGING_MAX_FILESIZE, OPENMITTSU_LOGGING_MAX_FILECOUNT)) {
		return -2;
	}

	// Activate Scaling on High-DPI Devices.
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#else
	qputenv("QT_DEVICE_PIXEL_RATIO", "auto");
#endif
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
	

	LOGGER()->info("Starting OpenMittsu {}...", Version::longVersionString());

	int result = 0;
	try {
		ExceptionHandlingApplication app(argc, argv);
		qRegisterMetaType<PublicKey>();
		qRegisterMetaType<ContactId>();
		qRegisterMetaType<QSet<ContactId>>();
		qRegisterMetaType<GroupId>();
		qRegisterMetaType<MessageId>();
		qRegisterMetaType<MessageTime>();
		qRegisterMetaType<FullMessageHeader>();
		qRegisterMetaType<QSharedPointer<FullMessageHeader const>>();
		qRegisterMetaType<PreliminaryContactMessage>();
		qRegisterMetaType<PreliminaryGroupMessage>();

		// Initialize Libraries now that Qt Dialogs are available.
		if (!initializeLibSodium()) {
			return -3;
		}

		if (!initializeFonts()) {
			return -4;
		}

		// Initialize the OptionMaster
		OptionMaster::getInstance()->registerOptions();

		// Load the app icon
		QIcon appIcon(":/icons/icon.png");
		app.setWindowIcon(appIcon);

		LOGGER()->debug("Creating QMainWindow instance.");
		Client client;
		client.show();
		LOGGER()->debug("Starting event loop.");
		result = app.exec();
	} catch (std::exception& e) {
		ExceptionHandlingApplication::displayExceptionInfo(&e, false);
		result = -1;
	}

	return result;
}

#ifdef _MSC_VER
int __stdcall WinMain(struct HINSTANCE__ *hInstance, struct HINSTANCE__ *hPrevInstance, char *lpszCmdLine, int nCmdShow) {
	return main(__argc, __argv);
}

#endif