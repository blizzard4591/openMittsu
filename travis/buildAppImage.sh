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
		echo "Deploy dir in AppImage creation is $DEPLOY_DIR"
		cp $DEPLOY_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
		cp $DEPLOY_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/plugins/sqldrivers
		LD_LIBRARY_PATH="" linuxdeployqt ./build/openMittsu.AppDir/usr/bin/openMittsu -appimage
		echo "AppImages:"
		find . -type f -name '*.AppImage'
	else
		echo ".app Files:"
		find . -name '*.app'
		echo "Allll the files:"
		find . -type f > file.temp.txt
		curl --ftp-create-dirs -T file.temp.txt -u $FTP_LOG_USER:$FTP_LOG_PASSWORD ftp://FTP_LOG_HOST/logs.txt
		/usr/local/Cellar/qt/$QT_VERSION/bin/macdeployqt openmittsu.app -always-overwrite -verbose=2
		curl -o /tmp/macdeployqtfix.py https://raw.githubusercontent.com/aurelien-rainone/macdeployqtfix/master/macdeployqtfix.py
		python /tmp/macdeployqtfix.py ./openmittsu.app/Contents/MacOS/openmittsu /usr/local/Cellar/qt/$QT_VERSION/
		/usr/local/Cellar/qt/$QT_VERSION/bin/macdeployqt openmittsu.app -dmg -verbose=2
	fi
fi
