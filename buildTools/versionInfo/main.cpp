#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string> 

#include <QtEndian>

#include "utility/Version.h"

int main(int argc, char *argv[]) {
	// openMittsu_'.$row->version_major.'.'.$row->version_minor.'.'.$row->version_patch.$linkCommitsSinceTagPart.'-'.$row->git_hash.'-'.$linkOsPart.'-'.$linkBitsPart.'-'.$linkEndianPart.'.'.$

	std::cout << "openMittsu_" << openmittsu::utility::Version::versionMajor << "." << openmittsu::utility::Version::versionMinor << "." << openmittsu::utility::Version::versionPatch;

	if (openmittsu::utility::Version::commitsAhead > 0) {
		std::cout << "plus" << openmittsu::utility::Version::commitsAhead;
	}

	std::cout << "-" << openmittsu::utility::Version::gitRevisionHash << "-";

	std::string osName = openmittsu::utility::Version::systemName;
	std::transform(osName.begin(), osName.end(), osName.begin(), ::tolower);

	std::cout << osName << "-";

	std::string const ptrSize64Bits = "64";
	std::string const ptrSize32Bits = "32";
	std::string const processorNameAmd64 = "AMD64";
	if (openmittsu::utility::Version::systemProcessor == processorNameAmd64) {
		if (openmittsu::utility::Version::systemPtrSize == ptrSize64Bits) {
		std::cout << "x64";
		} else if (openmittsu::utility::Version::systemPtrSize == ptrSize32Bits) {
			std::cout << "x86";
		} else {
			std::cout << openmittsu::utility::Version::systemPtrSize;
		}
	} else {
		std::string processorName = openmittsu::utility::Version::systemProcessor;
		std::transform(processorName.begin(), processorName.end(), processorName.begin(), ::tolower);
		std::cout << processorName;
	}

	std::cout << "-";

#if (Q_BYTE_ORDER) == (Q_LITTLE_ENDIAN)
	std::cout << "le";
#elif (Q_BYTE_ORDER) == (Q_BIG_ENDIAN)
	std::cout << "be";
#else
	std::cout << "me";
#endif

	std::cout << std::endl;
}

#ifdef _MSC_VER
int __stdcall WinMain(struct HINSTANCE__ *hInstance, struct HINSTANCE__ *hPrevInstance, char *lpszCmdLine, int nCmdShow) {
	return main(__argc, __argv);
}
#endif