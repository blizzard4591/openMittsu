#!/usr/bin/env bash

# Call this with QSQLCIPHER_DIR=/your/path/to/where/libqsqlcipher.so/resides

# set shell settings (see https://sipb.mit.edu/doc/safe-shell/)
set -eufv -o pipefail

mkdir -p ./build/openMittsu.AppDir/usr/bin
mkdir -p ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
mkdir -p ./build/openMittsu.AppDir/usr/plugins/sqldrivers
cp ./build/openMittsu ./build/openMittsu.AppDir/usr/bin
cp ./build/openMittsuTests ./build/openMittsu.AppDir/usr/bin
cp ./build/openMittsuVersionInfo ./build/openMittsu.AppDir/usr/bin
cp ./resources/icon.svg ./build/openMittsu.AppDir/openmittsu.svg
cp ./appImage/openmittsu.desktop ./build/openMittsu.AppDir/openmittsu.desktop

echo "Directoy for qt5-sqlcipher in AppImage creation is $QSQLCIPHER_DIR"
cp $QSQLCIPHER_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
cp $QSQLCIPHER_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/plugins/sqldrivers

wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage

if [ -f "linuxdeployqt" ] ; then
    rm "linuxdeployqt"
fi
mv linuxdeployqt-continuous-x86_64.AppImage linuxdeployqt
chmod +x linuxdeployqt

LD_LIBRARY_PATH="" ./linuxdeployqt ./build/openMittsu.AppDir/usr/bin/openMittsu -appimage
echo "AppImages:"
find . -type f -name '*.AppImage'
