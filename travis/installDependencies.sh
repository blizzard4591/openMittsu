#!/usr/bin/env bash

CURRENT_DIR=`pwd`
mkdir -p deploy
export DEPLOY_DIR=$CURRENT_DIR/deploy
echo "Deploy directory is: $DEPLOY_DIR"

if [ $TRAVIS_OS_NAME = linux ]; then
	sudo add-apt-repository ppa:ondrej/php -y
	sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
	if [ -n "${QT_PPA}" ]
	then
	sudo add-apt-repository "${QT_PPA}" -y
	sudo apt-get update -q
	sudo apt-get install -y "${QT_BASE}3d" "${QT_BASE}base" "${QT_BASE}graphicaleffects" "${QT_BASE}imageformats" "${QT_BASE}multimedia" "${QT_BASE}svg" "${QT_BASE}tools" "${QT_BASE}translations" "${QT_BASE}webengine"
	source "/opt/${QT_BASE}/bin/${QT_BASE}-env.sh"
	else
	sudo apt-get update -q
	sudo apt-get install -y qt5-default qttools5-dev-tools libqt5core5a libqt5gui5 libqt5multimedia5 libqt5multimedia5-plugins libqt5sql5 libqt5sql5-sqlite libqt5widgets5 qt5-qmake qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtbase5-dev qtbase5-private-dev
	fi

	sudo apt-get install -yq  libqrencode-dev git g++ libssl-dev make cmake pkgconf libsodium-dev libsodium18 zlib1g zlib1g-dev openssl
	sudo apt-get install -yq g++-6
	sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 90

	which $CC
	which $CXX
	# workaround clang not system wide, fail on sudo make install for adms
	export CC=`which $CC`
	export CXX=`which $CXX`
	
	mkdir build-dependencies
	pushd build-dependencies

		git clone https://github.com/sqlcipher/sqlcipher.git
		pushd sqlcipher
			./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC" --prefix=/usr
			make
			sudo make install
		popd

		git clone https://github.com/blizzard4591/qt5-sqlcipher.git
		pushd qt5-sqlcipher
			mkdir build
			cd build
			cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DQSQLCIPHER_COPYTO_DIR=$DEPLOY_DIR
			make
			./qsqlcipher-test
			sudo make install
			echo "Files in Deploy Dir:"
			find $DEPLOY_DIR -type f
		popd

	popd

	if [ "${DEPLOY_APPIMAGE}" = "true" ]
	then
		sudo wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" -O /usr/local/bin/linuxdeployqt
		sudo chmod a+x /usr/local/bin/linuxdeployqt
	fi
else
	# OS X
	brew install libsodium
	brew install qt5
	brew install sqlcipher
	brew install qrencode
	brew link --force libsodium
	brew link --force qt5
	brew link --force sqlcipher
	brew link --force qrencode

	export PATH="/usr/local/opt/qt/bin:$PATH"
	export QT_VERSION=`qmake -v | grep -Eo [0-9]+\\.[0-9]+\\.[0-9]+ | tail -1`
	export Qt5Core_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Core_DIR"
	export Qt5Gui_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Gui_DIR"
	export Qt5Widgets_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Widgets_DIR"
	export Qt5Network_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Network_DIR"
	export Qt5Multimedia_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Multimedia_DIR"
	export Qt5Sql_DIR="/usr/local/Cellar/qt/$QT_VERSION/lib/cmake:$Qt5Sql_DIR"

	mkdir build-dependencies
	pushd build-dependencies

	git clone https://github.com/blizzard4591/qt5-sqlcipher.git
	pushd qt5-sqlcipher
		mkdir build
		cd build
		echo "QT_VERSION = $QT_VERSION"
		echo "Path for Copyto is: /usr/local/Cellar/qt/$QT_VERSION/plugins/sqldrivers"
		cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DQSQLCIPHER_COPYTO_DIR="/usr/local/Cellar/qt/$QT_VERSION/plugins/sqldrivers"
		sudo make
		sudo ./qsqlcipher-test
		sudo make install
		popd
		echo "Mac QSqlCipher files:"
		find . -type f
	popd
fi

cd $CURRENT_DIR
