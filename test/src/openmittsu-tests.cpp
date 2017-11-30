#include <iostream>

#include "gtest/gtest.h"

#include <QCoreApplication>

#define OPENMITTSU_TESTS
#include "Init.h"

int main(int argc, char* argv[]) {
	std::cout << "OpenMittsu Testing Suite" << std::endl;
	
	if (!initializeLogging(OPENMITTSU_LOGGING_MAX_FILESIZE, OPENMITTSU_LOGGING_MAX_FILECOUNT)) {
		return -2;
	}

	int result = 0;
	try {
		testing::InitGoogleTest(&argc, argv);

		OPENMITTSU_REGISTER_TYPES();
		QCoreApplication application(argc, argv);

		// Initialize Libraries now that Qt Dialogs are available.
		if (!initializeLibSodium()) {
			return -3;
		}

		LOGGER()->debug("Starting tests...");
		int result = RUN_ALL_TESTS();

		if (result == 0) {
			std::cout << std::endl << "ALL TESTS PASSED!" << std::endl;
		} else{
			std::cout << std::endl << "TESTS FAILED!" << std::endl;
		}
	} catch (std::exception&) {
		result = -1;
	}

	return result;
}
