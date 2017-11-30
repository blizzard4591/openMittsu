#!/usr/bin/env bash

# set shell settings (see https://sipb.mit.edu/doc/safe-shell/)
set -eufv -o pipefail

# build AppImage
if [ "${DEPLOY_APPIMAGE-}" = "true" ]
then
	mkdir -p ./build/openMittsu.AppDir/usr/bin
	cp ./build/openMittsu ./build/openMittsu.AppDir/usr/bin
	cp ./build/openMittsuTests ./build/openMittsu.AppDir/usr/bin
	cp ./build/openMittsuVersionInfo ./build/openMittsu.AppDir/usr/bin
	LD_LIBRARY_PATH="" linuxdeployqt ./build/openMittsu.AppDir/usr/bin/openMittsu -appimage
fi
