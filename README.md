# Comet: Programming Environment for Lua  
(C) 2010-2022  Pr. Sidi HAMADY  
http://www.hamady.org  
sidi@hamady.org  

Released under the MIT licence (https://opensource.org/licenses/MIT)  
See Copyright Notice in COPYRIGHT 

Comet is a programming environment for the Lua scripting language.
It is available for Android, Linux and Windows with built-in Lua scripting engine, full-featured editor with syntax highlighting, code completion, inline documentation, script console, Lua samples and code snippets, debugging capabilities, etc. 

The main goal of Comet is to provide an easy-to-use development environment for the Lua programming language on Android, Linux and Windows. It integrates the Lua scripting engine with all Lua functionalities. Within the Comet environment, you can develop algorithms for science and engineering with one of the most elegant and fast scripting languages.


## Install  

To install Comet just copy the portable binary distribution from the Dist\OS directory to any location (such as usb key) and run comet (located in the bin directory). The zipped binary distribution is also available to download here: [Comet](http://www.hamady.org).


## Build  

I did some modifications on the wxWidgets library (and Scintilla), and, to a lesser extent, to LuaJIT.
All the modifications are marked with the [:COMET:] item and can then be easily traced in the code.
The modifications in the LuaJIT code are very light and can be very quicky applied, if necessary, to a new upgraded version.
The modifications in wxWidgets are far more numerous and need more work when upgraded to wxWidgets 3.x.

### Directories  

* Dist: where the binary distributions are located (For Windows and Linux, 32bit and 64bit)
* Main: Source code with:
    * the build directory
    * the C++ code 
* Manual: documentation
* output: where are located the binaries and intermediary object files

### Build under Linux  

For now, I used a Debian 7 machine to build the Comet release version.
to build Comet: cd to Main/build/gnu and start build.sh
* the script will clean, start other build scripts and make the release distribution.
* to build only Comet, just type: build.sh comet
* the build.sh was made for production.
* for developement, it is necessay to keep object and lib files and then comment out the clean step in the build.sh scripts.

### Build under Windows  

I still use Visual C++ 2013.
All the build process is handled by the batch file build.bat located in Main/build/win.  
build.bat need to be adapted and the the solutions (*.sln and *.vcxproj) need to be upgraded to the installed Visual C++ release.  
I use the MSBuild command line tool to build solutions. I do not use directly Visual C++ IDE.

