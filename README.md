ColaAne
=======

Adobe ANE extension for Cola libraries

This library is a [native extension for Adobe AIR](http://www.adobe.com/devnet/air/native-extensions-for-air.html) that works as a wrapper around [MArVL](http://marvl.infotech.monash.edu.au/)'s [libcola library](http://www.adaptagrams.org/).

Build instructions
------------------

You will need the [Adobe Flex SDK](http://www.adobe.com/devnet/flex/flex-sdk-download.html), the [Adobe AIR SDK](http://www.adobe.com/devnet/air/air-sdk-download.html) and [Apache Ant](http://ant.apache.org/). Visual Studio and Xcode are assumed as build tools, but any compiler should work as long as it can produce dynamic libraries with all the functions in `colaAne.cpp` exported. 

### Windows ###

Add the FlashRuntimeExtensions header and library files from the Adobe SDK to your include and library paths respectively. Open the colaAne.sln solution in Visual Studio and build the project. No other configuration should be necessary.

### Mac OS X ###

Edit the my\_cola.xcconfig file and replace all occurences of "path\_to\_air\_sdk" with the actual path to where you put the Adobe AIR SDK.

Go to the `cplusplus/ColaAneMac/colaAne` directory and build the library using `/usr/bin/xcodebuild -project ./colaAne.xcodeproj -xcconfig my_cola.xcconfig -configuration Release ARCHS="i386" build`

### Packaging the native extension ###

Set the `FLEX_HOME` environment variable to where your Adobe Flex SDK is located. Edit the `build.properties` file to add path to your keystore and its password.

Run ant with the appropriate buildfile - for example if you want to build the extension with only Windows support, you would run `ant -f build-win-only.xml`. If you wish to build the extension for both Windows and Mac OS X, you will need to copy the dynamic libraries built in the previous steps over to the computer where you will be packaging the extension (whether that is a Windows machine or a Mac OS X machine is irrelevant). The extension will be located in the `dist` directory.