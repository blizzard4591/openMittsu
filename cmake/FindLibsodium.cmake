# Once done these will be defined:
#
# LIBSODIUM_FOUND
# LIBSODIUM_INCLUDE_DIRS
# LIBSODIUM_LIBRARIES
#
 
if(LIBSODIUM_INCLUDE_DIRS AND LIBSODIUM_LIBRARIES)
	set(LIBSODIUM_FOUND TRUE)
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
 
	find_path(SODIUM_INCLUDE_DIR
		NAMES sodium.h
		HINTS
			ENV sodiumPath
			${_SODIUM_INCLUDE_DIRS}
			/usr/include /usr/local/include /opt/local/include /sw/include
	)
	
	set(_hints "")
 
	find_library(SODIUM_LIB
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${SODIUM_INCLUDE_DIR}/../lib ${SODIUM_INCLUDE_DIR}/../lib64 ${SODIUM_INCLUDE_DIR}/lib${_lib_suffix} ${SODIUM_INCLUDE_DIR}/../Build/Release/${_lib_suffix_win} ${_SODIUM_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
	
	find_library(SODIUM_LIB_DEBUG
		NAMES ${LIB_PREFIX}sodium.${LIB_POSTFIX}
		HINTS ${SODIUM_INCLUDE_DIR}/../lib ${SODIUM_INCLUDE_DIR}/../lib64 ${SODIUM_INCLUDE_DIR}/lib${_lib_suffix} ${SODIUM_INCLUDE_DIR}/../Build/Debug/${_lib_suffix_win} ${_SODIUM_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
 
	set(LIBSODIUM_INCLUDE_DIRS ${SODIUM_INCLUDE_DIR} CACHE PATH "libSodium include dir")
	set(LIBSODIUM_LIBRARIES "optimized;${SODIUM_LIB};debug;${SODIUM_LIB_DEBUG}" CACHE STRING "libSodium libraries")
 
	find_package_handle_standard_args(Libsodium DEFAULT_MSG SODIUM_LIB SODIUM_INCLUDE_DIR)
	mark_as_advanced(LIBSODIUM_INCLUDE_DIRS LIBSODIUM_LIBRARIES)
endif()
