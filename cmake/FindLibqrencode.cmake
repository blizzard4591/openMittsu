#
# FindLibQrEncode
#
# This module finds the libqrencode header and library files and extracts their version.  It
# sets the following variables.
#
# LIBQRENCODE_INCLUDE_DIR -  Include directory for libqrencode header files.
# LIBQRENCODE_LIBRARY -	The name of the library to link against.
#
# Libqrencode_FOUND - Indicates whether libqrencode has been found
#

find_path(LIBQRENCODE_INCLUDE_DIR qrencode.h
	HINTS
	$ENV{LIBQRENCODE_DIR}
	PATH_SUFFIXES include/qrencode include
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/opt/local
	/opt
)

FIND_LIBRARY(LIBQRENCODE_LIBRARY 
  NAMES qrencode
  HINTS
  $ENV{LIBQRENCODE_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /sw
  /opt/local
  /opt
)

if(LIBQRENCODE_INCLUDE_DIR)
	list(REMOVE_DUPLICATES LIBQRENCODE_INCLUDE_DIR)
endif(LIBQRENCODE_INCLUDE_DIR)

if(LIBQRENCODE_LIBRARY AND LIBQRENCODE_INCLUDE_DIR)
	set(Libqrencode_FOUND "YES")
endif(LIBQRENCODE_LIBRARY AND LIBQRENCODE_INCLUDE_DIR)

set(LIBQRENCODE_INCLUDE_DIRS ${LIBQRENCODE_INCLUDE_DIR})
mark_as_advanced(LIBQRENCODE_INCLUDE_DIR)
mark_as_advanced(LIBQRENCODE_LIBRARY)
