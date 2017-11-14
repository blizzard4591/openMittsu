#include "Init.h"
#include "Client.h"

#include "src/utility/ExceptionHandlingApplication.h"
#include "src/utility/OptionMaster.h"
#include "src/utility/Version.h"

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
	
	// Set encoding
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	LOGGER()->info("Starting OpenMittsu {}...", openmittsu::utility::Version::longVersionString());

	int result = 0;
	try {
		openmittsu::utility::ExceptionHandlingApplication app(argc, argv);
		OPENMITTSU_REGISTER_TYPES();

		// Initialize Libraries now that Qt Dialogs are available.
		if (!initializeLibSodium()) {
			return -3;
		}

		if (!initializeFonts()) {
			return -4;
		}

		// Load the app icon
		QIcon appIcon(":/icons/icon.png");
		app.setWindowIcon(appIcon);

		LOGGER()->debug("Creating QMainWindow instance.");
		Client client;
		client.show();
		LOGGER()->debug("Starting event loop.");
		result = app.exec();
	} catch (std::exception& e) {
		openmittsu::utility::ExceptionHandlingApplication::displayExceptionInfo(&e, false);
		result = -1;
	}

	return result;
}

#ifdef _MSC_VER
int __stdcall WinMain(struct HINSTANCE__ *hInstance, struct HINSTANCE__ *hPrevInstance, char *lpszCmdLine, int nCmdShow) {
	return main(__argc, __argv);
}

#endif
