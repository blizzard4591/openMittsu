#include <iostream>

#include "gtest/gtest.h"

#include "Init.h"
#include "utility/ExceptionHandlingApplication.h"

int main(int argc, char* argv[]) {
	std::cout << "OpenMittsu Testing Suite" << std::endl;
	
	if (!initializeLogging(OPENMITTSU_LOGGING_MAX_FILESIZE, OPENMITTSU_LOGGING_MAX_FILECOUNT)) {
		return -2;
	}
	
	int result = 0;
	try {
		testing::InitGoogleTest(&argc, argv);
		
		openmittsu::utility::ExceptionHandlingApplication app(argc, argv);
		OPENMITTSU_REGISTER_TYPES();

		// Initialize Libraries now that Qt Dialogs are available.
		if (!initializeLibSodium()) {
			return -3;
		}

		if (!initializeFonts()) {
			return -4;
		}

		LOGGER()->debug("Starting tests...");
		int result = RUN_ALL_TESTS();

		if (result == 0) {
			std::cout << std::endl << "ALL TESTS PASSED!" << std::endl;
		} else{
			std::cout << std::endl << "TESTS FAILED!" << std::endl;
		}
	} catch (std::exception& e) {
		openmittsu::utility::ExceptionHandlingApplication::displayExceptionInfo(&e, false);
		result = -1;
	}

    return result;
}
