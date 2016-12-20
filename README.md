# openMittsu
A cross-platform open source implementation and desktop client for the Threema Messenger App.

![A Screenshot of openMittsu](/screenshot1.jpg?raw=true "Chatting in openMittsu")

## License
openMittsu is governed by the GNU GPL v2.0 license, but includes works from different people, licensed under different terms. See [`LICENSE`](LICENSE) for more information.

## Requirements
 - [CMake](https://cmake.org/)
 - [Qt 5](https://www.qt.io/)
 - [LibSodium](https://download.libsodium.org/doc/)
 - [LibQrEncode](https://fukuchi.org/works/qrencode/) or [LibQrEncode-win32](https://code.google.com/p/qrencode-win32/)
 
## Supported Platforms
Currently, the application has been built and tested on:
 - Windows 7 using Visual Studio 2013
 - Windows 7 using Visual Studio 2015
 - Windows 10 using Visual Studio 2015
 - Debian 8 using GCC 4.8 (AMD64)
 - Debian 8 using GCC 4.9 (PPC)
 
Other platforms should work with no or minimal changes.

## How to build
1. Clone this repository or extract the sources from the archive.
	If you clone the repository, do not forget to intialize the submodules (i.e. external git repositories):
```
git submodule init
git submodule sync
git submodule update --init --recursive
```

2. Install or build both LibSodium (```apt-get install libsodium-dev```) and LibQrEncode (```apt-get install libqrencode-dev```)
3. Run CMake on the main directory. Point CMake to the installation locations of libSodium and LibQrEncode, if required, using `LIBSODIUM_INCLUDE_DIRS`, `LIBSODIUM_LIBRARIES`, `LIBQRENCODE_INCLUDE_DIR` and `LIBQRENCODE_LIBRARY`, respectively.
4. Once Makefiles or a solution (MSVS) has been successfully generated, start the build process.
	On Windows, choosing a DEBUG configuration causes the application to show an additional terminal window containinig run-time debug information and logs.

Prerequisites on Debian/Ubuntu: 
```
apt-get install libqt5core5a libqt5gui5 libqt5multimedia5 libqt5multimedia5-plugins libqt5sql5 libqt5sql5-sqlite libqt5widgets5 qt5-qmake qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev libsodium-dev libqrencode-dev git
```

### Detailed steps

Adjust the paths to your system. The ones below are a current Qt version installed on OSX via brew.
```bash
cd my-projects/ # or wherever you want to clone this
git checkout https://github.com/blizzard4591/openMittsu.git
cd openMittsu
mkdir build
cd build
# If you are on Mac OSX (install Qt5 using Homebrew, and then find the suitable paths for your installation)
export Qt5Core_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Core/
export Qt5Gui_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Gui/
export Qt5Widgets_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Widgets/
export Qt5Network_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Network/
export Qt5Multimedia_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Multimedia

cmake .. # or cmake .. -G Xcode, optionally add -DOPENMITTSU_DEBUG=On
make # optionally add -j 4 for threaded compilation on with 4 threads
./openMittsu
```

## How to use
The application requires two input files.
 - The contact database. This is a simple text file of the format:
```
# This is a comment line.
# Format of this file: 
# IDENTITY : PUBKEY : Nickname
# where 
# - IDENTITY is an eight character ID of the form [A-Z0-9]{8} and stands for a users public id,
# - PUBKEY is an 64 character key of the form [a-fA-F0-9]{64} and stands for a users 32-Byte long-term public key,
# - Nickname is an optional screen-name for the given identity.
# You can also find IDENTITY and PUBKEY of your contacts in the contacts.csv of a Threema data backup.

ABCD1234 : aabbccddeeff00112233445566778899aabbccddeeff00112233445566778899 : Mr. Smith

# GROUPID : GROUPOWNER_IDENTITY : IDENTITY, IDENTITY, IDENTITY : Group Name
# where 
# - IDENTITY is an eight character ID of the form [A-Z0-9]{8} and stands for a users public id,
# - GROUPID is an 16 character key of the form [a-fA-F0-9]{16} and stands for a groups unique identifier,
# - GROUPOWNER_IDENTITY is an eight character ID of the form [A-Z0-9]{8} and stands for the group owners public id,
# - Group Name is the displayed title of the group.
# You can also find IDENTITY, GROUPID and GROUPOWNER_IDENTITY in the groups.csv of a Threema data backup.
```
 - The client configuration, containing the ID and private key used by this client.
 It can easily be generated using the application.
   1. Export the identity from the Threema App. To do this, in the Threema App-menu, select "My Backups" and then create an ID Backup. 
   After entering a password, a string of the form "AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA" is displayed.
   2. In openMittsu, click on Identity, Load Backup.
   3. Enter the generated backup string and the password.
   4. Choose a suitable location and let the application save the generated client configuration there.

## FAQ
### What is openMittsu?
openMittsu is a cross-platform open source implementation and desktop client for the Threema Messenger App. It has been created by reverse engineering the Threema mobile App and studying the publication from [Jan Ahrens](http://blog.jan-ahrens.eu/2014/03/22/threema-protocol-analysis.html) as well as the Threema Gateway.

### Can I use the same ID for openMittsu and Threema?
The protocol Threema uses doesn't provide multi-device functionality (yet) which means you can't be connected with more than one device using the same ID simultaneously. However, if you disconnect your phone before connecting with openMittsu, you can use the same Threema ID for your phone and openMittsu. Please beware that you will only receive messages on one device.
Alternatively, consult the Threema FAQ-answer on this topic: [Can I install Threema on multiple devices?](https://threema.ch/en/faq/multidevice)

### Can I generate a Threema ID using openMittsu?
No, please use the Threema app you purchased for generating an ID. You can use Threema on a second user on your phone or in an emulator if you need to as long as you have a legally purchased Threema app.

### Does the Threema GmbH allow this project?
Yes, Threema tolerates the openMittsu project. However, if openMittsu will get abused, Threema may change their decision. Don't worry, your ID won't get banned just for using openMittsu (if you are using openMittsu just for instant messaging like you do with the Threema app).

### Is the Threema GmbH involved in the development?
No. The Threema GmbH is currently not interested in working on openMittsu.

### Can I use openMittsu for automated messages?
No, if you want to send or receive Threema messages automatically, please use the official [Threema Gateway API](https://gateway.threema.ch/).

### Does openMittsu replace an official web client that Threema may be working on?
No. While a web client will connect to Threema through your phone, openMittsu is a standalone client which does not rely on your phone for connecting to the Threema servers.
If you are interested in how a Threema web client could work, read lgrahl's thesis about [SaltyRTC](http://lgrahl.de/pub/ba-thesis-saltyrtc-by-lennart-grahl-revised-v1.pdf).
  
  
## Acknowledgement
Without the initial research and publication by [Jan Ahrens](http://blog.jan-ahrens.eu/2014/03/22/threema-protocol-analysis.html) this project would have been so much more difficult, if not impossible.
