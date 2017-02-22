cmake_minimum_required (VERSION 2.8.11)

# Set project name
project (openMittsu CXX C)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/")

# SpdLog base path, included via GIT Submodule
SET(SPDLOG_MODULE_BASEPATH "${PROJECT_SOURCE_DIR}/3rdparty/spdlog")

# SpdLog Configuration via #defines
add_definitions(-DSPDLOG_DEBUG_ON)

# CMake Portable Endian base path, included via GIT Submodule
SET(CMAKEPORTABLEENDIAN_MODULE_BASEPATH "${PROJECT_SOURCE_DIR}/3rdparty/cmake-portable-endian")

option(OPENMITTSU_LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(OPENMITTSU_USE_LIBCXX "Sets whether the standard library is libc++." OFF)
option(OPENMITTSU_DEBUG "Sets whether debug checks, assertions and logging should be turned on. Has no effect on builds under MSVC besides turning on debug logging level." OFF)
option(OPENMITTSU_DISABLE_VERSION_UPDATE_CHECK "Disables the version check on start-up. Useful for custom builds or added privacy." OFF)
SET(OPENMITTSU_CMAKE_SEARCH_PATH "D:/Qt/Qt5.7.0/5.7/msvc2015_64" CACHE PATH "Additional Qt5 search path" )
set(OPENMITTSU_CUSTOM_VERSION_STRING "" CACHE STRING "Disables Git version number checking and uses the custom string instead. Should be of the form 0.1.2-34-567890ab, i.e. major.minor.patch-commitsSinceTag-shortHash")

SET(CMAKE_PREFIX_PATH "${OPENMITTSU_CMAKE_SEARCH_PATH}/lib/cmake/")

if (OPENMITTSU_DEBUG)
	SET(OPENMITTSU_CONFIG_DEBUG_FLAG_SET ON)
	if (MSVC)
		message(WARNING "Setting the DEBUG flag has no effect on MSVC besides turning on DEBUG logging.")
	endif(MSVC)
	message(STATUS "Configuring OpenMittsu to be build in debug mode.")
else(OPENMITTSU_DEBUG)
	SET(OPENMITTSU_CONFIG_DEBUG_FLAG_SET OFF)
endif(OPENMITTSU_DEBUG)

if (OPENMITTSU_DISABLE_VERSION_UPDATE_CHECK)
	SET(OPENMITTSU_CONFIG_DISABLE_VERSION_UPDATE_CHECK ON)
	message(STATUS "Version update checks on start-up will be disabled.")
else(OPENMITTSU_DISABLE_VERSION_UPDATE_CHECK)
	SET(OPENMITTSU_CONFIG_DISABLE_VERSION_UPDATE_CHECK OFF)
endif(OPENMITTSU_DISABLE_VERSION_UPDATE_CHECK)

# Find the Qt library
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Libsodium REQUIRED)
find_package(Libqrencode REQUIRED)

if (LIBSODIUM_FOUND)
	message(STATUS "Found LibSodium! ${LIBSODIUM_INCLUDE_DIRS} ${LIBSODIUM_LIBRARIES}")
	# Add LibSodium Includes
	include_directories(${LIBSODIUM_INCLUDE_DIRS})
else ()
	message(SEND_ERROR "Can not build OpenMittsu without LibSodium.")
endif(LIBSODIUM_FOUND)

if (Libqrencode_FOUND)
	message(STATUS "Found LibQREncode with include directory: ${LIBQRENCODE_INCLUDE_DIR}")
	SET(OPENMITTSU_CONFIG_HAVE_LIBQRENCODE ON)
	# Add libQrEncode Includes
	include_directories(${LIBQRENCODE_INCLUDE_DIR})
else (Libqrencode_FOUND)
	message(STATUS "Could not find LibQREncode, disabling QR Code features.")
	SET(OPENMITTSU_CONFIG_HAVE_LIBQRENCODE OFF)
endif(Libqrencode_FOUND)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Add SpgLog Includes
include_directories("${SPDLOG_MODULE_BASEPATH}/include")

# Add Portable Endian
add_subdirectory("${CMAKEPORTABLEENDIAN_MODULE_BASEPATH}" "${PROJECT_BINARY_DIR}/cmakePortableEndian")
include_directories("${PROJECT_BINARY_DIR}/cmakePortableEndian")

if(CMAKE_COMPILER_IS_GNUCC)
    set(OPENMITTSU_COMPILED_BY "GCC")
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
elseif(MSVC)
    set(OPENMITTSU_COMPILED_BY "MSVC")
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
	
	add_definitions(/DSODIUM_STATIC)
	add_definitions(/DNOMINMAX)
	add_definitions(/DWIN32_LEAN_AND_MEAN)
	
	# Disable "C:\Program Files (x86)\Windows Kits\8.1\Include\um\DbgHelp.h(1544): warning C4091: 'typedef ': ignored on left of '' when no variable is declared"
	add_definitions(/wd4091)
else(CLANG)
    set(OPENMITTSU_COMPILED_BY "Clang (LLVM)")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O3")
    if(UNIX AND NOT APPLE AND NOT OPENMITTSU_USE_LIBCXX)
		set(CLANG_STDLIB libstdc++)
    else()
		set(CLANG_STDLIB libc++)
		# Set up some Xcode specific settings
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    endif()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-unused-variable -ftemplate-depth=1024")
endif()

#
# Make a version file containing the current version from git.
#

# First check if this is a Git checkout or if a custom version tag should be used.
if ((NOT "${OPENMITTSU_CUSTOM_VERSION_STRING}" STREQUAL "") AND (NOT "${OPENMITTSU_CUSTOM_VERSION_STRING}" MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+(\\-.*)?$"))
	message(FATAL_ERROR "Builtin version information: A custom version string is set, but it is malformed. Should be: 0.1.2-34-567890ab, i.e. major.minor.patch-commitsSinceTag-shortHash")
endif()

set(OPENMITTSU_GIT_VERSION_STRING "")
if (NOT "${OPENMITTSU_CUSTOM_VERSION_STRING}" STREQUAL "")
	message(STATUS "Builtin version information: custom version string set, using ${OPENMITTSU_CUSTOM_VERSION_STRING}")
	set(OPENMITTSU_GIT_VERSION_STRING "${OPENMITTSU_CUSTOM_VERSION_STRING}")
elseif (NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message(STATUS "Builtin version information: .git folder non-existant, falling back to 0.0.0-0-00000000")
	set(OPENMITTSU_GIT_VERSION_STRING "0.0.0-0-00000000")
else()
	message(STATUS "Builtin version information: .git folder exists, using git_describe_checkout")
	include(GetGitRevisionDescription)
	git_describe_checkout(OPENMITTSU_GIT_VERSION_STRING)
endif()

if ("${OPENMITTSU_GIT_VERSION_STRING}" MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+(\\-.*)?$")
	# Parse the git Tag into variables
	string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" OPENMITTSU_CPP_VERSION_MAJOR "${OPENMITTSU_GIT_VERSION_STRING}")
	string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" OPENMITTSU_CPP_VERSION_MINOR "${OPENMITTSU_GIT_VERSION_STRING}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" OPENMITTSU_CPP_VERSION_PATCH "${OPENMITTSU_GIT_VERSION_STRING}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" OPENMITTSU_CPP_VERSION_COMMITS_AHEAD "${OPENMITTSU_GIT_VERSION_STRING}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" OPENMITTSU_CPP_VERSION_HASH "${OPENMITTSU_GIT_VERSION_STRING}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" OPENMITTSU_CPP_VERSION_APPENDIX "${OPENMITTSU_GIT_VERSION_STRING}")
	if ("${OPENMITTSU_CPP_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
		set(OPENMITTSU_CPP_VERSION_DIRTY 1)
	else()
		set(OPENMITTSU_CPP_VERSION_DIRTY 0)
	endif()
message(STATUS "OpenMittsu - Version information: ${OPENMITTSU_CPP_VERSION_MAJOR}.${OPENMITTSU_CPP_VERSION_MINOR}.${OPENMITTSU_CPP_VERSION_PATCH} (${OPENMITTSU_CPP_VERSION_COMMITS_AHEAD} commits ahead of Tag) build from ${OPENMITTSU_CPP_VERSION_HASH} (Dirty: ${OPENMITTSU_CPP_VERSION_DIRTY})")
else()
	message(STATUS "No GIT support, no tags fetched or other problems. Defaulting to version 0.0.0-0-00000000 (result was ${OPENMITTSU_GIT_VERSION_STRING})")
	set(OPENMITTSU_CPP_VERSION_MAJOR 0)
	set(OPENMITTSU_CPP_VERSION_MINOR 0)
	set(OPENMITTSU_CPP_VERSION_PATCH 0)
	set(OPENMITTSU_CPP_VERSION_COMMITS_AHEAD 0)
	set(OPENMITTSU_CPP_VERSION_HASH "00000000")
	set(OPENMITTSU_CPP_VERSION_DIRTY 0)
endif()

# Check for %zu support
try_run(OPENMITTSU_POINTER_SIZE_TEST_RUN_RESULT OPENMITTSU_POINTER_SIZE_TEST_COMPILE_RESULT "${PROJECT_BINARY_DIR}/testPointerSize" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_ptr_size.cpp" COMPILE_OUTPUT_VARIABLE OPENMITTSU_POINTER_SIZE_TEST_COMPILE_OUTPUT RUN_OUTPUT_VARIABLE OPENMITTSU_POINTER_SIZE_TEST_RUN_OUTPUT)
if (NOT ${OPENMITTSU_POINTER_SIZE_TEST_COMPILE_RESULT})
	message(FATAL_ERROR "Failed to compile pointer size testing program, please contact a developer! Extended Information: ${OPENMITTSU_POINTER_SIZE_TEST_COMPILE_OUTPUT}") 
elseif(${OPENMITTSU_POINTER_SIZE_TEST_RUN_RESULT} LESS 0 OR ${OPENMITTSU_POINTER_SIZE_TEST_RUN_RESULT} GREATER 0)
	message(FATAL_ERROR "Failed to run pointer size testing program, please contact a developer! Extended Information: ${OPENMITTSU_POINTER_SIZE_TEST_RUN_OUTPUT}") 
else()
	set(OPENMITTSU_CPP_POINTER_SIZE "${OPENMITTSU_POINTER_SIZE_TEST_RUN_OUTPUT}")
	message(STATUS "Collected pointer size information (pointer has ${OPENMITTSU_POINTER_SIZE_TEST_RUN_OUTPUT} Bits)")
endif()

# Configure a file to pass the OpenMittsu version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/Version.cpp.in"
	"${PROJECT_BINARY_DIR}/include/Version.cpp"
)

# Configure a header file to pass various configuration settings
configure_file (
	"${PROJECT_SOURCE_DIR}/Config.h.in"
	"${PROJECT_BINARY_DIR}/include/Config.h"
)

# Add the binary dir include directory
include_directories("${PROJECT_BINARY_DIR}/include")

# Main Sources
file(GLOB_RECURSE OPENMITTSU_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_CPP ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_ACKNOWLEDGMENTS ${PROJECT_SOURCE_DIR}/src/acknowledgments/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_ACKNOWLEDGMENTS ${PROJECT_SOURCE_DIR}/src/acknowledgments/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_CINTERFACE ${PROJECT_SOURCE_DIR}/src/c-interface/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_CINTERFACE ${PROJECT_SOURCE_DIR}/src/c-interface/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_CPPINTERFACE ${PROJECT_SOURCE_DIR}/src/cpp-interface/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_CPPINTERFACE ${PROJECT_SOURCE_DIR}/src/cpp-interface/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_GENERATED ${PROJECT_BINARY_DIR}/include/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_GENERATED ${PROJECT_BINARY_DIR}/include/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_EXCEPTIONS ${PROJECT_SOURCE_DIR}/src/exceptions/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_EXCEPTIONS ${PROJECT_SOURCE_DIR}/src/exceptions/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_MESSAGES_CONTACT ${PROJECT_SOURCE_DIR}/src/messages/contact/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_MESSAGES_CONTACT ${PROJECT_SOURCE_DIR}/src/messages/contact/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_MESSAGES_GROUP ${PROJECT_SOURCE_DIR}/src/messages/group/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_MESSAGES_GROUP ${PROJECT_SOURCE_DIR}/src/messages/group/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_MESSAGES ${PROJECT_SOURCE_DIR}/src/messages/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_MESSAGES ${PROJECT_SOURCE_DIR}/src/messages/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_PROTOCOL ${PROJECT_SOURCE_DIR}/src/protocol/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_PROTOCOL ${PROJECT_SOURCE_DIR}/src/protocol/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_TASKS ${PROJECT_SOURCE_DIR}/src/tasks/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_TASKS ${PROJECT_SOURCE_DIR}/src/tasks/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_UTILITY ${PROJECT_SOURCE_DIR}/src/utility/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_UTILITY ${PROJECT_SOURCE_DIR}/src/utility/*.cpp)

file(GLOB_RECURSE OPENMITTSU_HEADERS_WIZARDS ${PROJECT_SOURCE_DIR}/src/wizards/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_WIZARDS ${PROJECT_SOURCE_DIR}/src/wizards/*.cpp)
file(GLOB_RECURSE OPENMITTSU_FORMS_WIZARDS ${PROJECT_SOURCE_DIR}/src/wizards/*.ui)

file(GLOB_RECURSE OPENMITTSU_HEADERS_WIDGETS ${PROJECT_SOURCE_DIR}/src/widgets/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_WIDGETS ${PROJECT_SOURCE_DIR}/src/widgets/*.cpp)
file(GLOB_RECURSE OPENMITTSU_FORMS_WIDGETS ${PROJECT_SOURCE_DIR}/src/widgets/*.ui)

file(GLOB_RECURSE OPENMITTSU_HEADERS_DIALOGS ${PROJECT_SOURCE_DIR}/src/dialogs/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_DIALOGS ${PROJECT_SOURCE_DIR}/src/dialogs/*.cpp)
file(GLOB_RECURSE OPENMITTSU_FORMS_DIALOGS ${PROJECT_SOURCE_DIR}/src/dialogs/*.ui)

file(GLOB_RECURSE OPENMITTSU_HEADERS_UPDATER ${PROJECT_SOURCE_DIR}/src/updater/*.h)
file(GLOB_RECURSE OPENMITTSU_SOURCES_UPDATER ${PROJECT_SOURCE_DIR}/src/updater/*.cpp)
file(GLOB_RECURSE OPENMITTSU_FORMS_UPDATER ${PROJECT_SOURCE_DIR}/src/updater/*.ui)

file(GLOB_RECURSE OPENMITTSU_SOURCES_C ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE OPENMITTSU_FORMS ${PROJECT_SOURCE_DIR}/src/*.ui)
file(GLOB_RECURSE OPENMITTSU_RESOURCES ${PROJECT_SOURCE_DIR}/resources/*.qrc)

QT5_WRAP_UI(OPENMITTSU_FORMS_HEADERS ${OPENMITTSU_FORMS})
QT5_WRAP_UI(OPENMITTSU_FORMS_HEADERS_WIZARDS ${OPENMITTSU_FORMS_WIZARDS})
QT5_WRAP_UI(OPENMITTSU_FORMS_HEADERS_WIDGETS ${OPENMITTSU_FORMS_WIDGETS})
QT5_WRAP_UI(OPENMITTSU_FORMS_HEADERS_DIALOGS ${OPENMITTSU_FORMS_DIALOGS})
QT5_WRAP_UI(OPENMITTSU_FORMS_HEADERS_UPDATER ${OPENMITTSU_FORMS_UPDATER})
QT5_ADD_RESOURCES(OPENMITTSU_RESOURCESOURCES ${OPENMITTSU_RESOURCES})

# Sources of Build Tools
file(GLOB_RECURSE OPENMITTSU_BUILDTOOLS_VERSIONINFO_HEADERS ${PROJECT_SOURCE_DIR}/buildTools/versionInfo/*.h)
file(GLOB_RECURSE OPENMITTSU_BUILDTOOLS_VERSIONINFO_SOURCES_CPP ${PROJECT_SOURCE_DIR}/buildTools/versionInfo/*.cpp)

# Main Grouping
source_group(Headers FILES ${OPENMITTSU_HEADERS} ${OPENMITTSU_FORMS_HEADERS} ${OPENMITTSU_HEADERS_GENERATED})
source_group(Sources FILES ${OPENMITTSU_SOURCES_C} ${OPENMITTSU_SOURCES_CPP} ${OPENMITTSU_SOURCES_GENERATED})
source_group(Acknowledgments FILES ${OPENMITTSU_HEADERS_ACKNOWLEDGMENTS} ${OPENMITTSU_SOURCES_ACKNOWLEDGMENTS})
source_group(C-Interface FILES ${OPENMITTSU_HEADERS_CINTERFACE} ${OPENMITTSU_SOURCES_CINTERFACE})
source_group(Cpp-Interface FILES ${OPENMITTSU_HEADERS_CPPINTERFACE} ${OPENMITTSU_SOURCES_CPPINTERFACE})
source_group(Exceptions FILES ${OPENMITTSU_HEADERS_EXCEPTIONS} ${OPENMITTSU_SOURCES_EXCEPTIONS})
source_group(Messages FILES ${OPENMITTSU_HEADERS_MESSAGES} ${OPENMITTSU_SOURCES_MESSAGES})
source_group(Messages\\Contact FILES ${OPENMITTSU_HEADERS_MESSAGES_CONTACT} ${OPENMITTSU_SOURCES_MESSAGES_CONTACT})
source_group(Messages\\Group FILES ${OPENMITTSU_HEADERS_MESSAGES_GROUP} ${OPENMITTSU_SOURCES_MESSAGES_GROUP})
source_group(Protocol FILES ${OPENMITTSU_HEADERS_PROTOCOL} ${OPENMITTSU_SOURCES_PROTOCOL})
source_group(Tasks FILES ${OPENMITTSU_HEADERS_TASKS} ${OPENMITTSU_SOURCES_TASKS})
source_group(Utility FILES ${OPENMITTSU_HEADERS_UTILITY} ${OPENMITTSU_SOURCES_UTILITY})
source_group(Wizards FILES ${OPENMITTSU_HEADERS_WIZARDS} ${OPENMITTSU_SOURCES_WIZARDS} ${OPENMITTSU_FORMS_HEADERS_WIZARDS} ${OPENMITTSU_FORMS_WIZARDS})
source_group(Widgets FILES ${OPENMITTSU_HEADERS_WIDGETS} ${OPENMITTSU_SOURCES_WIDGETS} ${OPENMITTSU_FORMS_HEADERS_WIDGETS} ${OPENMITTSU_FORMS_WIDGETS})
source_group(Dialogs FILES ${OPENMITTSU_HEADERS_DIALOGS} ${OPENMITTSU_SOURCES_DIALOGS} ${OPENMITTSU_FORMS_HEADERS_DIALOGS} ${OPENMITTSU_FORMS_DIALOGS})
source_group(Updater FILES ${OPENMITTSU_HEADERS_UPDATER} ${OPENMITTSU_SOURCES_UPDATER} ${OPENMITTSU_FORMS_HEADERS_UPDATER} ${OPENMITTSU_FORMS_UPDATER})

# Add the executables
add_executable(openMittsu ${OPENMITTSU_HEADERS} ${OPENMITTSU_SOURCES_C} ${OPENMITTSU_SOURCES_CPP} ${OPENMITTSU_RESOURCESOURCES} ${OPENMITTSU_FORMS_HEADERS} 
	${OPENMITTSU_HEADERS_ACKNOWLEDGMENTS} ${OPENMITTSU_SOURCES_ACKNOWLEDGMENTS}
	${OPENMITTSU_HEADERS_GENERATED} ${OPENMITTSU_SOURCES_GENERATED}
	${OPENMITTSU_HEADERS_CINTERFACE} ${OPENMITTSU_SOURCES_CINTERFACE}
	${OPENMITTSU_HEADERS_CPPINTERFACE} ${OPENMITTSU_SOURCES_CPPINTERFACE}
	${OPENMITTSU_HEADERS_EXCEPTIONS} ${OPENMITTSU_SOURCES_EXCEPTIONS}
	${OPENMITTSU_HEADERS_MESSAGES} ${OPENMITTSU_SOURCES_MESSAGES}
	${OPENMITTSU_HEADERS_PROTOCOL} ${OPENMITTSU_SOURCES_PROTOCOL}
	${OPENMITTSU_HEADERS_TASKS} ${OPENMITTSU_SOURCES_TASKS}
	${OPENMITTSU_HEADERS_UTILITY} ${OPENMITTSU_SOURCES_UTILITY}
	${OPENMITTSU_FORMS_HEADERS_WIZARDS} ${OPENMITTSU_HEADERS_WIZARDS} ${OPENMITTSU_SOURCES_WIZARDS} 
	${OPENMITTSU_FORMS_HEADERS_WIDGETS} ${OPENMITTSU_HEADERS_WIDGETS} ${OPENMITTSU_SOURCES_WIDGETS}
	${OPENMITTSU_FORMS_HEADERS_DIALOGS} ${OPENMITTSU_HEADERS_DIALOGS} ${OPENMITTSU_SOURCES_DIALOGS} 
	${OPENMITTSU_HEADERS_UPDATER} ${OPENMITTSU_SOURCES_UPDATER} ${OPENMITTSU_FORMS_HEADERS_UPDATER}
)

add_executable(openMittsuVersionInfo ${OPENMITTSU_BUILDTOOLS_VERSIONINFO_HEADERS} ${OPENMITTSU_BUILDTOOLS_VERSIONINFO_SOURCES_CPP} ${OPENMITTSU_HEADERS_GENERATED} ${OPENMITTSU_SOURCES_GENERATED})

if (MSVC)
	set_target_properties(openMittsu PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif(MSVC)

# Add target link dependencies
target_link_libraries(openMittsu ${LIBSODIUM_LIBRARIES})
target_link_libraries(openMittsu ${LIBQRENCODE_LIBRARY})
message(STATUS "Linking ENDIAN libs: ${ENDIANNESS_LINK_LIBRARIES}")
target_link_libraries(openMittsu "${ENDIANNESS_LINK_LIBRARIES}")
target_link_libraries(openMittsuVersionInfo "${ENDIANNESS_LINK_LIBRARIES}")

# Use the required modules from Qt 5.
target_link_libraries(openMittsu Qt5::Core)
target_link_libraries(openMittsu Qt5::Gui)
target_link_libraries(openMittsu Qt5::Widgets)
target_link_libraries(openMittsu Qt5::Network)
target_link_libraries(openMittsu Qt5::Multimedia)

# Link against libc++abi if requested.
if (OPENMITTSU_LINK_LIBCXXABI)
	target_link_libraries(openMittsu "c++abi")
	target_link_libraries(openMittsuVersionInfo "c++abi")
endif(OPENMITTSU_LINK_LIBCXXABI)

# Targets, CPACK...

install(TARGETS openMittsu
   RUNTIME
   DESTINATION bin
   COMPONENT applications)

