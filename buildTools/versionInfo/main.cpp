#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string> 

#include "endianness_config.h"
#include "utility/Version.h"

int main(int argc, char *argv[]) {
	// openMittsu_'.$row->version_major.'.'.$row->version_minor.'.'.$row->version_patch.$linkCommitsSinceTagPart.'-'.$row->git_hash.'-'.$linkOsPart.'-'.$linkBitsPart.'-'.$linkEndianPart.'.'.$

	std::cout << "openMittsu_" << Version::versionMajor << "." << Version::versionMinor << "." << Version::versionPatch;

	if (Version::commitsAhead > 0) {
		std::cout << "plus" << Version::commitsAhead;
	}

	std::cout << "-" << Version::gitRevisionHash << "-";

	std::string osName = Version::systemName;
	std::transform(osName.begin(), osName.end(), osName.begin(), ::tolower);

	std::cout << osName << "-";

	std::string const ptrSize64Bits = "64";
	std::string const ptrSize32Bits = "32";
	if (Version::systemPtrSize == ptrSize64Bits) {
		std::cout << "x64";
	} else if (Version::systemPtrSize == ptrSize32Bits) {
		std::cout << "x86";
	} else {
		std::cout << Version::systemPtrSize;
	}

	std::cout << "-";

#if ENDIANNESS_CONFIG_ENDIAN_TYPE == ENDIANNESS_VAL_LITTLE_ENDIAN
	std::cout << "le";
#elif ENDIANNESS_CONFIG_ENDIAN_TYPE == ENDIANNESS_VAL_BIG_ENDIAN
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