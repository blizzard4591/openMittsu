#!/usr/bin/env bash

echo "0"
pwd

if [ $TRAVIS_OS_NAME = linux ]; then
  CURRENT_DIR=`pwd`
  sudo add-apt-repository ppa:ondrej/php -y
  sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
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

  sudo apt-get install -yq  libqrencode-dev git g++ libssl-dev make cmake pkgconf libsodium-dev libsodium18 zlib1g zlib1g-dev openssl
  sudo apt-get install -yq g++-6
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 90

  echo "1"
  pwd
  mkdir build-dependencies
  pushd build-dependencies
  echo "2"
  pwd

  git clone https://github.com/sqlcipher/sqlcipher.git
  pushd sqlcipher && ./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC" --prefix=/usr && make && sudo make install && popd
  echo "3"
  pwd

  git clone https://github.com/blizzard4591/qt5-sqlcipher.git
  pushd qt5-sqlcipher && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr && make && sudo make install && popd
  echo "4"
  pwd
  popd

  if [ "${DEPLOY_APPIMAGE}" = "true" ]
  then
    sudo wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" -O /usr/local/bin/linuxdeployqt
    sudo chmod a+x /usr/local/bin/linuxdeployqt
  fi
  echo "5"
  pwd
  cd $CURRENT_DIR
  echo "6"
  pwd
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
echo "7"
pwd

