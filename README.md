# Leviosa Motor Shades application

This repository contains object files of iOS and Android versions of Leviosa application and instructions for anyone who want to rebuild (relink to) the application with custom Qt5 builds in accordance to LGPL version 3.

**Leviosa Application** is a *Combined work* in terms of LGPL version 3.

## Licensing conditions

1. This software is produced with with using unchanged Qt5 Libraries under terms of LGPL version 3, 29 June 2007 <http://doc.qt.io/qt-5/lgpl.html>
2. CryptoPP software is used under terms of Boost Software License 1.0, August 17th 2003 <http://www.boost.org/users/license.html> 
3. Amazon AWS SDK for C++ is used under terms of Apache License Version 2.0, January 2004 <https://github.com/aws/aws-sdk-cpp/blob/master/LICENSE.txt>


## Rebuilding / Relinking with other Qt5 Library

Linked Qt5 Library version is 5.9.3 and it's sources accessible on <https://github.com/qt/qt5/tree/v5.9.3>

Custom builds of the linked version of Qt5 library can be used to get Combined Work by following instructions of this document for Android and iOS platforms.

# Android

The android application is distributing and built only for armv7 architecture.

Android Leviosa Application is linking dynamically with the Qt5 libraries. Modified Qt5 libraries can be used to comply requirement of 4.d.1 of LGPLv3. In order to get Combined work linking with new Qt5 libraries follwing step are required.

1. Download _leviosa.apk_ file from _android_ folder of this repository.
2. Rename _leviosa.apk_ to _leviosa.zip_
3. Upack _leviosa.zip_ file in some \<FOLDER>
4. Place new versions of Qt5 shared objects for armv7 architecture into \<FOLDER>/lib/armeabi-v7a
5. Pack the \<FOLDER> with zip archiver to _new\_levioza.zip_
6. Rename _new\_leviosa.zip_ to _new\_leviosa.apk_
7. The _new\_leviosa.apk_ is ready to setup on Android device and run.

# iOS

The android application is distributing and built only for armv7 architecture.
