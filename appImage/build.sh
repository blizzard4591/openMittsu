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
cp ./appImage/openmittsu.desktop ./build/openMittsu.AppDir/openMittsu.desktop

echo "Directoy for qt5-sqlcipher in AppImage creation is $QSQLCIPHER_DIR"
cp $QSQLCIPHER_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers
cp $QSQLCIPHER_DIR/libqsqlcipher.so ./build/openMittsu.AppDir/usr/plugins/sqldrivers

wget -c https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - | grep "appimagetool-.*-x86_64.AppImage" | head -n 1 | cut -d '"' -f 2)

if [ -f "appimagetool" ] ; then
    rm "appimagetool"
fi
mv appimagetool-*.AppImage appimagetool
chmod +x appimagetool

LD_LIBRARY_PATH="" ./appimagetool-*.AppImage -s deploy /build/openMittsu.AppDir/usr/share/applications/*.desktop # Bundle EVERYTHING
echo "AppImages:"
find . -type f -name '*.AppImage'
