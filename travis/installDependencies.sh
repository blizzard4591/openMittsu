#!/usr/bin/env bash

if [ $TRAVIS_OS_NAME = linux ]; then 
	sudo add-apt-repository ppa:ondrej/php -y
	if [ -n "${QT_PPA}" ]
	then
		sudo add-apt-repository "${QT_PPA}" -y
		sudo apt-get update -q
		sudo apt-get install -y "${QT_BASE}-meta-full"
		source "/opt/${QT_BASE}/bin/${QT_BASE}-env.sh"
	else
		sudo apt-get update -q
		sudo apt-get install -y qt5-default qttools5-dev-tools libqt5core5a libqt5gui5 libqt5multimedia5 libqt5multimedia5-plugins libqt5sql5 libqt5sql5-sqlite libqt5widgets5 qt5-qmake qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev qtbase5-dev qtbase5-private-dev
	fi

	sudo apt-get install -yqq  libqrencode-dev git g++ libssl-dev make cmake pkgconf libsodium-dev libsodium18 zlib1g zlib1g-dev openssl
	
	mkdir build-dependencies
	pushd build-dependencies
	
	git clone https://github.com/sqlcipher/sqlcipher.git
	pushd sqlcipher && ./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC" --prefix=/usr && make && sudo make install && popd
	
	git clone https://github.com/blizzard4591/qt5-sqlcipher.git
	pushd qt5-sqlcipher && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr && make && sudo make install && popd
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
fi

