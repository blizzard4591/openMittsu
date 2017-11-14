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
		pkg_check_modules(_SODIUM QUIET sodium)
	endif()
 
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
		set(_lib_suffix_win "x64")
	else()
		set(_lib_suffix 32)
		set(_lib_suffix_win "x86")
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
			${_SODIUM_INCLUDE_DIRS}
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
 
	find_library(SODIUM_LIB
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${Libsodium_INCLUDE_DIRS}/../lib ${Libsodium_INCLUDE_DIRS}/../lib64 ${Libsodium_INCLUDE_DIRS}/lib${_lib_suffix} ${Libsodium_INCLUDE_DIRS}/../Build/Release/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../${_lib_suffix_win}/Release/v141/ltcg ${_SODIUM_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
	
	find_library(SODIUM_LIB_DEBUG
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${Libsodium_INCLUDE_DIRS}/../lib ${Libsodium_INCLUDE_DIRS}/../lib64 ${Libsodium_INCLUDE_DIRS}/lib${_lib_suffix} ${Libsodium_INCLUDE_DIRS}/../Build/Debug/${_lib_suffix_win} ${Libsodium_INCLUDE_DIRS}/../${_lib_suffix_win}/Debug/v141/ltcg ${_SODIUM_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
 
	set(Libsodium_INCLUDE_DIRS ${Libsodium_INCLUDE_DIRS} CACHE PATH "libSodium include dir")
	if (SODIUM_LIB_DEBUG AND SODIUM_LIB)
		set(Libsodium_LIBRARIES "optimized;${SODIUM_LIB};debug;${SODIUM_LIB_DEBUG}" CACHE STRING "libSodium libraries")
	elseif(SODIUM_LIB)
		set(Libsodium_LIBRARIES "${SODIUM_LIB}" CACHE STRING "libSodium libraries")
	else()
		set(Libsodium_LIBRARIES "")
	endif()
 
	find_package_handle_standard_args(Libsodium FOUND_VAR Libsodium_FOUND REQUIRED_VARS Libsodium_LIBRARIES Libsodium_INCLUDE_DIRS)
	mark_as_advanced(Libsodium_INCLUDE_DIRS Libsodium_LIBRARIES)
endif()
