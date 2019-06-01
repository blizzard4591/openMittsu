# Once done these will be defined:
#
# Libsodium_FOUND
# Libsodium_NCLUDE_DIRS
# Libsodium_LIBRARIES
# Libsodium_MAJOR_VERSION    - LibSodium major version number (X in X.y.z)
# Libsodium_MINOR_VERSION    - LibSodium minor version number (Y in x.Y.z)
# Libsodium_SUBMINOR_VERSION - LibSodium subminor version number (Z in x.y.Z)
#
 
if(Libsodium_INCLUDE_DIRS AND Libsodium_LIBRARIES)
	set(Libsodium_FOUND TRUE)
else()
	find_package(PkgConfig QUIET)
	if(PKG_CONFIG_FOUND)
		pkg_check_modules(Libsodium QUIET libsodium)
		if (Libsodium_FOUND)
			set(_Libsodium_HAS_PKG_CONFIG ON)
			if (NOT Libsodium_INCLUDE_DIRS)
				set(Libsodium_INCLUDE_DIRS "${Libsodium_INCLUDEDIR}")
			endif()
		endif()
	endif()
 
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
		set(_lib_suffix_win "x64")
	else()
		set(_lib_suffix 32)
		set(_lib_suffix_win "Win32")
	endif()
	
	IF (WIN32)
		IF (MINGW)
			SET (LIB_PREFIX "lib")
			SET (LIB_POSTFIX "so")
		ELSEIF (MSVC)
			SET (LIB_PREFIX "lib")
			SET (LIB_POSTFIX "lib")
		ENDIF(MINGW)   
	ELSE (UNIX)
		SET (LIB_PREFIX "lib")
		SET (LIB_POSTFIX "so")
	ENDIF (WIN32)
 
	find_path(Libsodium_INCLUDE_DIRS
		NAMES sodium.h
		HINTS
			ENV sodiumPath
			${Libsodium_INCLUDE_DIRS}
			/usr/include /usr/local/include /opt/local/include /sw/include
	)
	
	if (Libsodium_INCLUDE_DIRS)
		# Extract SODIUM_VERSION_STRING from version.h
		set(Libsodium_VERSION 0)
		file(STRINGS "${Libsodium_INCLUDE_DIRS}/sodium/version.h" _sodium_VERSION_H_CONTENTS REGEX "#define SODIUM_VERSION_STRING ")
		set(_sodium_VERSION_REGEX "\"([0-9]+)\\.([0-9]+)\\.([0-9]+)\"")
		if("${_sodium_VERSION_H_CONTENTS}" MATCHES "#define SODIUM_VERSION_STRING ${_sodium_VERSION_REGEX}")
			set(Libsodium_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
		endif()
		unset(_sodium_VERSION_H_CONTENTS)
	
		set(Libsodium_MAJOR_VERSION "${CMAKE_MATCH_1}")
		set(Libsodium_MINOR_VERSION "${CMAKE_MATCH_2}")
		set(Libsodium_SUBMINOR_VERSION "${CMAKE_MATCH_3}")
		
		if(Libsodium_FIND_VERSION)
			# Set Libsodium_FOUND based on requested version.
			if("${Libsodium_VERSION}" VERSION_LESS "${Libsodium_FIND_VERSION}")
				set(Libsodium_FOUND 0)
				message(STATUS "The installed version of libsodium is too old (requested: ${Libsodium_FIND_VERSION}, found: ${Libsodium_VERSION})")
			elseif(Libsodium_FIND_VERSION_EXACT AND NOT "${Libsodium_VERSION}" VERSION_EQUAL "${Libsodium_FIND_VERSION}")
				set(Libsodium_FOUND 0)
				message(STATUS "The installed version of libsodium is too new (requested: ${Libsodium_FIND_VERSION}, found: ${Libsodium_VERSION})")
			else()
				set(Libsodium_FOUND 1)
			endif()
		else()
			# Caller will accept any Boost version.
			set(Libsodium_FOUND 1)
		endif()
	endif()
	
	set(_hints "")

	set(MSVC_TOOLSET_VERSION "v142")
	if(MSVC_VERSION)
		if (((MSVC_VERSION GREATER 1920) OR (MSVC_VERSION EQUAL 1920)) AND (MSVC_VERSION LESS 1930))
			set(MSVC_TOOLSET_VERSION "v142")
		elseif (((MSVC_VERSION GREATER 1910) OR (MSVC_VERSION EQUAL 1910)) AND (MSVC_VERSION LESS 1920))
			set(MSVC_TOOLSET_VERSION "v141")
		elseif (MSVC_VERSION EQUAL 1900)
			set(MSVC_TOOLSET_VERSION "v140")
		elseif (MSVC_VERSION EQUAL 1800)
			set(MSVC_TOOLSET_VERSION "v120")
		elseif (MSVC_VERSION EQUAL 1700)
			set(MSVC_TOOLSET_VERSION "v110")
		elseif (MSVC_VERSION EQUAL 1600)
			set(MSVC_TOOLSET_VERSION "v100")
		elseif (MSVC_VERSION EQUAL 1500)
			set(MSVC_TOOLSET_VERSION "v90")
		elseif (MSVC_VERSION EQUAL 1400)
			set(MSVC_TOOLSET_VERSION "v80")
		else ()
			message(STATUS "The MSVC_VERSION is '${MSVC_VERSION}'! This version is unknown, defaulting to v142 (VS2019).")
		endif()
	endif()

	find_library(SODIUM_LIB
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${Libsodium_INCLUDE_DIRS}/../lib ${Libsodium_INCLUDE_DIRS}/../lib64 ${Libsodium_INCLUDE_DIRS}/lib${_lib_suffix} ${Libsodium_INCLUDE_DIRS}/../Build/Release/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../../../Build/Release/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../${_lib_suffix_win}/Release/${MSVC_TOOLSET_VERSION}/static ${Libsodium_LIBRARY_DIRS} ${Libsodium_LIBRARIES} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
	
	find_library(SODIUM_LIB_DEBUG
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${Libsodium_INCLUDE_DIRS}/../lib ${Libsodium_INCLUDE_DIRS}/../lib64 ${Libsodium_INCLUDE_DIRS}/lib${_lib_suffix} ${Libsodium_INCLUDE_DIRS}/../Build/Debug/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../../../Build/Debug/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../${_lib_suffix_win}/Debug/${MSVC_TOOLSET_VERSION}/static ${Libsodium_LIBRARY_DIRS} ${Libsodium_LIBRARIES} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
 
	set(Libsodium_INCLUDE_DIRS ${Libsodium_INCLUDE_DIRS} CACHE PATH "libSodium include dir")
	if (SODIUM_LIB_DEBUG AND SODIUM_LIB)
		set(Libsodium_LIBRARIES "optimized;${SODIUM_LIB};debug;${SODIUM_LIB_DEBUG}" CACHE STRING "libSodium libraries")
	elseif(SODIUM_LIB)
		set(Libsodium_LIBRARIES "${SODIUM_LIB}" CACHE STRING "libSodium libraries")
	else()
		if (NOT _Libsodium_HAS_PKG_CONFIG)
			set(Libsodium_LIBRARIES "")
		endif()
	endif()
 
	find_package_handle_standard_args(Libsodium FOUND_VAR Libsodium_FOUND REQUIRED_VARS Libsodium_LIBRARIES Libsodium_INCLUDE_DIRS)
	mark_as_advanced(Libsodium_INCLUDE_DIRS Libsodium_LIBRARIES)
endif()
