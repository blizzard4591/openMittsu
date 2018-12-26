# Once done these will be defined:
#
# Libqrencode_FOUND
# Libqrencode_INCLUDE_DIRS
# Libqrencode_LIBRARIES
#
 
if(Libqrencode_INCLUDE_DIRS AND Libqrencode_LIBRARIES)
	set(Libqrencode_FOUND TRUE)
else()
	find_package(PkgConfig QUIET)
	if(PKG_CONFIG_FOUND)
		pkg_check_modules(Libqrencode QUIET libqrencode)
		if (Libqrencode_FOUND)
			set(_Libqrencode_HAS_PKG_CONFIG ON)
			if (NOT Libqrencode_INCLUDE_DIRS)
				set(Libqrencode_INCLUDE_DIRS "${Libqrencode_INCLUDEDIR}")
			endif()
		endif()
	endif()
 
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
		set(_lib_suffix_win "x64")
		set(_lib_suffix_win_alt "x64")
	else()
		set(_lib_suffix 32)
		set(_lib_suffix_win "Win32")
		set(_lib_suffix_win_alt "x86")
	endif()
	
	IF (WIN32)
		IF (MINGW)
			SET (LIB_PREFIX "lib")
			SET (LIB_POSTFIX "so")
		ELSEIF (MSVC)
			SET (LIB_PREFIX "")
			SET (LIB_POSTFIX "lib")
		ENDIF(MINGW)   
	ELSE (UNIX)
		SET (LIB_PREFIX "lib")
		SET (LIB_POSTFIX "so")
	ENDIF (WIN32)
 
	find_path(Libqrencode_INCLUDE_DIRS
		NAMES qrencode.h
		HINTS
			ENV qrencodePath
			${Libqrencode_INCLUDE_DIRS}
			/usr/include /usr/local/include /opt/local/include /sw/include
	)
	
	if (Libqrencode_INCLUDE_DIRS)
		set(Libqrencode_FOUND 1)
	endif()
	
	set(_hints "")
 
	find_library(QRENCODE_LIB
		NAMES ${LIB_PREFIX}qrencode.${LIB_POSTFIX} ${LIB_PREFIX}qrcodelib.${LIB_POSTFIX}
		HINTS ${Libqrencode_INCLUDE_DIRS}/../lib ${Libqrencode_INCLUDE_DIRS}/../lib64 ${Libqrencode_INCLUDE_DIRS}/lib${_lib_suffix} ${Libqrencode_INCLUDE_DIRS}/../compile/${_lib_suffix_win_alt}/Release ${Libqrencode_INCLUDE_DIRS}/vc15/${_lib_suffix_win}/Release-Lib ${Libqrencode_LIBRARY_DIRS} ${Libqrencode_LIBRARIES} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
	# message(STATUS "Looking at ${LIB_PREFIX}qrencode.${LIB_POSTFIX}, found = ${QRENCODE_LIB}, considering path = ${Libqrencode_INCLUDE_DIRS}/../compile/${_lib_suffix_win_alt}/Release")
	
	find_library(QRENCODE_LIB_DEBUG
		NAMES ${LIB_PREFIX}qrencode.${LIB_POSTFIX} ${LIB_PREFIX}qrcodelib.${LIB_POSTFIX}
		HINTS ${Libqrencode_INCLUDE_DIRS}/../lib ${Libqrencode_INCLUDE_DIRS}/../lib64 ${Libqrencode_INCLUDE_DIRS}/lib${_lib_suffix} ${Libqrencode_INCLUDE_DIRS}/../compile/${_lib_suffix_win_alt}/Debug ${Libqrencode_INCLUDE_DIRS}/vc15/${_lib_suffix_win}/Debug-Lib ${Libqrencode_LIBRARY_DIRS} ${Libqrencode_LIBRARIES} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
	)
	# message(STATUS "Looking at ${LIB_PREFIX}qrencode.${LIB_POSTFIX}, found = ${QRENCODE_LIB_DEBUG}")
 
	set(Libqrencode_INCLUDE_DIRS ${Libqrencode_INCLUDE_DIRS} CACHE PATH "libQrEncode include dir")
	if (QRENCODE_LIB_DEBUG AND QRENCODE_LIB)
		set(Libqrencode_LIBRARIES "optimized;${QRENCODE_LIB};debug;${QRENCODE_LIB_DEBUG}" CACHE STRING "libQrEncode libraries")
	elseif(QRENCODE_LIB)
		set(Libqrencode_LIBRARIES "${QRENCODE_LIB}" CACHE STRING "libQrEncode libraries")
	else()
		if (NOT _Libqrencode_HAS_PKG_CONFIG)
			set(Libqrencode_LIBRARIES "")
		endif()
	endif()
 
	find_package_handle_standard_args(Libqrencode FOUND_VAR Libqrencode_FOUND REQUIRED_VARS Libqrencode_LIBRARIES Libqrencode_INCLUDE_DIRS)
	# mark_as_advanced(Libqrencode_INCLUDE_DIRS Libqrencode_LIBRARIES)
endif()
