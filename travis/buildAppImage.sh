#!/usr/bin/env bash

# set shell settings (see https://sipb.mit.edu/doc/safe-shell/)
set -eufv -o pipefail

# build AppImage
if [ "${DEPLOY_APPIMAGE-}" = "true" ]
then
  if [ $TRAVIS_OS_NAME = linux ]; then
    mkdir -p ./build/openMittsu.AppDir/usr/bin
	mkdir -p ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
	mkdir -p ./build/openMittsu.AppDir/usr/plugins/sqldrivers
    cp ./build/openMittsu ./build/openMittsu.AppDir/usr/bin
    cp ./build/openMittsuTests ./build/openMittsu.AppDir/usr/bin
    cp ./build/openMittsuVersionInfo ./build/openMittsu.AppDir/usr/bin
    cp ./resources/icon.svg ./build/openMittsu.AppDir/openmittsu.svg
    cp ./travis/openmittsu.desktop ./build/openMittsu.AppDir/openmittsu.desktop
	cp /usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers/libqsqlcipher.so ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
	cp /usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers/libqsqlcipher.so ./build/openMittsu.AppDir/usr/plugins/sqldrivers
    LD_LIBRARY_PATH="" linuxdeployqt ./build/openMittsu.AppDir/usr/bin/openMittsu -appimage
	echo "AppImages:"
    find . -type f -name '*.AppImage'
  else
    echo "Allll the files:"
    find . -type f
  fi
fi
