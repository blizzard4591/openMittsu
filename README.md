# openMittsu
A cross-platform open source implementation and desktop client for the Threema Messenger App.

![A Screenshot of openMittsu](/screenshot1.jpg?raw=true "Chatting in openMittsu")

## License
openMittsu is governed by the GNU GPL v2.0 license, but includes works from different people, licensed under different terms. See [`LICENSE`](LICENSE) for more information.

## Requirements
 - [CMake](https://cmake.org/)
 - [Qt 5](https://www.qt.io/)
 - [LibSodium >= 1.0.12](https://download.libsodium.org/doc/)
 - [LibQrEncode](https://fukuchi.org/works/qrencode/) or [LibQrEncode-win32](https://code.google.com/p/qrencode-win32/)
 - [LibSqlCipher >= 3.4.1](https://github.com/blizzard4591/sqlcipher)
 - [QSqlCipher](https://github.com/blizzard4591/qt5-sqlcipher)
 
## Supported Platforms
Currently, the application has been built and tested on:
 - Windows 7 using Visual Studio 2017
 - Windows 10 using Visual Studio 2017
 - Debian 9 using GCC 6.3 (AMD64)
 - Debian 9 using GCC 6.3 (PPC)
 
Other platforms should work with no or minimal changes.

## Binaries
Currently we only provide binaries for  the following platforms:
 - [Windows 32bit (x86) and 64bit (x64)](https://downloads.openmittsu.de/) – Scroll down to find the latest build. Do not forget to install the Visual Studio Redistributable (`vcredist_x64.exe`).
 - Debian 9 amd64 using our APT repository:

	1. First, add the GPG key for the repository:
		```wget -O - https://packages.openmittsu.de/openmittsu.packages.gpg.key|apt-key add -```
	2. Now, add the repository. As root or using sudo, create a file called "openmittsu.list" in `/etc/apt/sources.list.d/` with the following contents:
		```deb https://packages.openmittsu.de/apt/debian stretch main```
	3. You can now install (or update) openMittsu using:
		```sudo apt-get update```
		```sudo apt-get install openmittsu```

## How to use
The application stores your ID, contacts and messages in a database.
A new database can be created using the "Load Backup" Wizard available from Database > Load Backup.
You can either import an ID backup (containing only your ID and private key, no contacts or messages), or import a full data backup.
 - ID Backup:
   1. Export the identity from the Threema App. To do this, in the Threema App-menu, select "My Backups" and then create an ID Backup.  
   After entering a password, a string of the form "AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA-AAAA" is displayed.
   2. In openMittsu, click on Database, Load Backup, ID Backup.
   3. Enter the generated backup string and the password.
   4. Enter a secure password for the database. If SqlCipher support is available, the database is encrypted using this password.
   5. Choose a suitable location (an empty folder) and let the application save the generated database there.
 - Data Backup:
	1. Export the data from the Threema App. To do this, in the Threema App-menu, select "My Backups" and then create a Data Backup.  
	After entering a password and waiting for the backup to complete, transfer the backup to your computer.  
	Now, extract the backup (a .zip file) to a secure folder (in your home directory for example) using the chosen backup password. 7zip or WinRAR can do this on Windows.
	2. In openMittsu, click on Database, Load Backup, Data Backup.
	3. Point openMittsu to where you extracted the backup and enter the chosen backup password.
	4. Enter a secure password for the database. If SqlCipher support is available, the database is encrypted using this password.
	5. Choose a suitable location (an empty folder) and let the application save the generated database there.

Load the created database from the central GUI and point the file dialog to the directory where you chose to store the database.  
After entering the password, the file should be displayed as loaded and active and contacts etc. should appear, if there are any.  
Click Connect to connect to the Threema servers and chat!

## How to build

Find detailed steps for Linux and Windows systems below.

### General steps

1. Clone this repository or extract the sources from the archive.  
	If you clone the repository, do not forget to intialize the submodules (i.e. external git repositories):

		git submodule init
		git submodule sync
		git submodule update --init --recursive

2. Install or build both `libsodium` (for example ```apt-get install libsodium-dev```) and `libqrencode` (for example ```apt-get install libqrencode-dev```).  
	Note that openMittsu requires at least `libsodium` 1.0.12 or newer.
3. Run CMake on the main directory. Point CMake to the installation locations of `libsodium` and `libqrencode`, if required, using `LIBSODIUM_INCLUDE_DIRS`, `LIBSODIUM_LIBRARIES`, `LIBQRENCODE_INCLUDE_DIR` and `LIBQRENCODE_LIBRARY`, respectively.
4. Once Makefiles or a solution (MSVS) has been successfully generated, start the build process.  
	On Windows, choosing a `DEBUG` configuration causes the application to show an additional terminal window containinig run-time debug information and logs.
    
    Adding support for encrypted databases using `sqlcipher`: This is where it gets messy. You need the Qt5 private development headers (```apt-get install qtbase5-dev qtbase5-private-dev```) and libsqlcipher (```apt-get install libsqlcipher0 libsqlcipher-dev```).
	
    You need to make sure that you are using the latest version of `libsqlcipher` (at this time 3.4.1) – if not, you will get `SEGMENTATION FAULTS`. Debian Stretch currently still ships version 3.2.0, which is **too old**! This version still links against OpenSSL 1.0.x, which is incompatible with the version of OpenSSL linked by Qt.
    
	A (patched) version of `libsqlcipher` which also builds under Windows (build using ```nmake -f Makefile.msc```, adapt paths in Makefile.msc to Windows SDK on variable TCC and RCC, adapt paths to OpenSSL binaries (use [these non-light installers](https://slproweb.com/products/Win32OpenSSL.html)) at all locations referencing OpenSSL) [is available here](https://github.com/blizzard4591/sqlcipher).
    
	Now you need to build the [wrapper between SqlCipher and Qt](https://github.com/blizzard4591/qt5-sqlcipher).
    
	Clone the repository (```git clone https://github.com/blizzard4591/qt5-sqlcipher.git```), configure with cmake (create `build/` directory in the cloned folder, execute ```cmake ..```).
    
	If you are using Windows, you first need to change two entries in CMakeLists.txt:  
	There are two lines marked with `# Change this by hand if needed`, right below the variables `SQLCIPHER_INCLUDE_DIRS` and `SQLCIPHER_LIBRARIES` are defined.  
	Update the paths to point to your checkout of sqlcipher (INCLUDE_DIRS) and to your sqlite3.lib file created by nmake:
        
		set(SQLCIPHER_INCLUDE_DIRS "C:/somePath/sqlcipher")
		set(SQLCIPHER_LIBRARIES "C:/somePath/sqlcipher/sqlite3.lib")
        
	Now build `QSqlCipher`. If everything went well there should be a library in `sqldrivers/`.
    
	You can run ```qsqlcipher-test``` to see if your version of SqlCipher produces Segfaults or not – if you do not get past Test 5.2, update your installed version of `libsqlcipher`.
    
	The library created here (and `sqlite3.dll` in the case of Windows) need to be in openMittsu's main directory (the QSqlCipher module MUST be in a subfolder named `sqldrivers`!).
    
	On Linux, you also have the option of installing QSqlCipher as a global Qt5 plugin. On Debian 9 x64, the system-wide storage for plugins is in ```/usr/lib/x86_64-linux-gnu/qt5/plugins```. For copying the plugin there you need root privileges. The command would look like ```sudo cp ./sqldrivers/libqsqlcipher.so /usr/lib/x86_64-linux-gnu/qt5/plugins/sqldrivers/```.
	
Prerequisites on Debian/Ubuntu: 
```
apt-get install libqt5core5a libqt5gui5 libqt5multimedia5 libqt5multimedia5-plugins libqt5sql5 libqt5sql5-sqlite libqt5widgets5 qt5-qmake qtbase5-dev qtbase5-dev-tools qtmultimedia5-dev libqrencode-dev git g++ libssl-dev make cmake qtbase5-dev qtbase5-private-dev pkgconf
```
Important: If you are using Debian 9: To install a more recent version of `libsodium` (at least 1.0.12) and a fixed version of SqlCipher, you need to have the openMittsu repository in your APT configuration.
To do this safely, add ```deb https://packages.openmittsu.de/apt/debian stretch main``` as described in the section on binaries.

After running ```apt-get update```, you can install the additional packages using:

	apt-get install libsodium-dev libsodium23 libsqlcipher0 libsqlcipher-dev

Prerequisites on openSuse (incomplete): 

	sudo zypper install libqt5-qtbase-devel libqt5-qtmultimedia-devel libsodium-devel qrencode-devel git g++ cmake

### Detailed steps – Linux

Adjust the paths to your system. The ones below are a current Qt version installed on OSX via brew.
Look above for hints on how to add SqlCipher (encrypted database storage) support!
```bash
cd my-projects/ # or wherever you want to clone this

# If you are on Mac OSX (install Qt5 using Homebrew, and then find the suitable paths for your installation)
export Qt5Core_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Core/
export Qt5Gui_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Gui/
export Qt5Widgets_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Widgets/
export Qt5Network_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Network/
export Qt5Multimedia_DIR=/usr/local/Cellar/qt5/5.7.0/lib/cmake/Qt5Multimedia

# First build QSqlCipher
git clone https://github.com/blizzard4591/qt5-sqlcipher.git
cd qt5-sqlcipher
mkdir build
cd build
# If you are on Windows, the next step required some more parameters to cmake, aka the paths to SqlCipher include and library.
cmake .. # or cmake .. -G Xcode
make
# Either later on copy the created library in sqldrivers/ by hand to openMittsu, or install into system wide Qt5 plugin folders (look for the location of libqsqlite.so)

# Build openMittsu
git clone https://github.com/blizzard4591/openMittsu.git
cd openMittsu
mkdir build
cd build

cmake .. # or cmake .. -G Xcode, optionally add -DOPENMITTSU_DEBUG=On
make # optionally add -j 4 for multi-threaded compilation with 4 threads
./openMittsu
```


### Detailed steps – Windows

1. Install [CMake](https://cmake.org/download/)
2. [Download the latest version of `libsodium`](https://download.libsodium.org/libsodium/releases/)  
	Either pick the sources and compile them yourself, or use the provided binaries for MSVC in the -msvc.zip packages.
	Assuming you use the precompiled version and extracted them to `C:\cpp\libsodium-1.0.xx-msvc`, where xx is the most recent version, the values for CMake later on will be:  
		`LIBSODIUM_LIBRARIES = optimized;C:\cpp\libsodium-1.0.xx-msvc\x64\Release\v141\static\libsodium.lib;debug;C:\cpp\libsodium-1.0.xx-msvc\x64\Debug\v141\static\libsodium.lib` and 
		`LIBSODIUM_INCLUDE_DIRS = C:\cpp\libsodium-1.0.xx-msvc\include`.
3. Clone and build the [`qrencode library`](https://github.com/blizzard4591/qrencode-win32)  
	Use the solution provided in `vc15/` and build only the target `qrcodelib` in modes `Debug-Lib` and `Release-Lib`.
	The basepath where `qrencode.h` is located will serve as the include directory `LIBQRENCODE_INCLUDE_DIR` and
	we define `LIBQRENCODE_LIBRARY = optimized;C:\cpp\qrencode-win32\qrencode-win32\vc15\x64\Release-Lib\qrcodelib.lib;debug;C:\cpp\qrencode-win32\qrencode-win32\vc15\x64\Debug-Lib\qrcodelib.lib` (change pathes according to your layout!).
4. Download and install the 8latest Qt version](https://www.qt.io/download-open-source/). Check that you select the right version for your version of Visual Studio, for example msvc-2017-x64 for Visual Studio 2017 and 64bit builds.
5. Open the `CMakeLists.txt` file in the root folder of openMittsu and look for `OPENMITTSU_CMAKE_SEARCH_PATH`.  
	Edit the path to point to your Qt installation.
6. Start CMake and point it to the openMittsu directory.
	Lets say you cloned openMittsu to `C:\cpp\openMittsu`, then fill out the two lines in CMake as follows:
		
        Where is the source code: C:/cpp/openMittsu
		Where to build the binaries: C:/cpp/openMittsu/build
        
	Using the `/build` folder allows us to perform an out-of-source build that does not pollute the sources with build files.  
	You can even have simultaneous x86 and x64 builds from the same source, just by using different build directories.  
	Click `Configure` and select the appropriate generator for Visual Studio, for example `Visual Studio 15 2017 Win64` for Visual Studio 2017 in 64bit mode.  
	After the first configuration run, there will be errors.  
	Insert the values `LIBSODIUM_LIBRARIES`, `LIBSODIUM_INCLUDE_DIRS`, `LIBQRENCODE_LIBRARY` and `LIBQRENCODE_INCLUDE_DIR` like discussed above.  
	Click `Configure` again, and no more errors should appear – otherwise fill an issue.  
	Click `Generate` and open the generated solution file in the build directory.  
	You can now build either a Debug or a Release build of openMittsu by using the standard Visual Studio target selection.
7. Look above for hints on how to add SqlCipher (encrypted database storage) support!	

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

### What's the difference between openMittsu and [Threema Web](https://github.com/threema-ch/threema-web)?
While [Threema Web](https://github.com/threema-ch/threema-web) relies on a working connection to your Android smartphone running Threema, openMittsu acts as a standalone client. You can use openMittsu without a phone.
Also, openMittsu is a native application, not a JavaScript / browser based web application.
  
  
## Acknowledgement
Without the initial research and publication by [Jan Ahrens](http://blog.jan-ahrens.eu/2014/03/22/threema-protocol-analysis.html) this project would have been so much more difficult, if not impossible.
