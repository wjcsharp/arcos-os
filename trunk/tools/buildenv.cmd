@echo off

rem ***************************************************************************
rem Set these variables when setting up the build environment
rem ***************************************************************************
set BASEDIR=\osprojekt

rem Visual C++ specific directories
set MSVCHOMEDIR=C:\Program Files (x86)\Microsoft Visual Studio 9.0
set WINSDKDIR=C:\Program Files\Microsoft SDKs\Windows\v6.0A



rem **************************************************************************
rem No customization needed from this point
rem **************************************************************************

set TOOLSDIR=%BASEDIR%\tools
set PATH=%TOOLSDIR%;%PATH%


rem Default target is mipsel32
set TARGET=%1
if "%TARGET%"=="" set TARGET=mipsel32


rem Makefiles use this to decide between POSIX/Windows environment
set WINDOWS_HOST=1


if /I "%TARGET%" NEQ "mipsel32" goto win32
rem ***************************************************************************
rem MIPS32 little endian
rem ***************************************************************************
set MIPSEL32=1
set COMPILER=gcc
set PATH=%TOOLSDIR%\gnu\mips-idt-elf\bin;%PATH%
goto targetset

:win32
rem if /I "%TARGET%" NEQ "win32" goto ...
rem ***************************************************************************
rem Win32 hosted process
rem ***************************************************************************
set WIN32=1
set COMPILER=msvc
set PATH=%MSVCHOMEDIR%\Common7\IDE;%MSVCHOMEDIR%\VC\bin;%PATH%
set INCLUDE=%MSVCHOMEDIR%\VC\include;%WINSDKDIR%\include
set LIBPATH=%MSVCHOMEDIR%\VC\LIB;%WINSDKDIR%\lib
set LIB=%LIBPATH%
goto targetset


rem ***************************************************************************
rem Other targets go here
rem ***************************************************************************



:targetset


rem ***************************************************************************
rem Set up paths for GNU tools
rem ***************************************************************************
set PATH=%TOOLSDIR%\gnu\bin;%PATH%

cd %BASEDIR%

rem when set = checked build, when unset = free build
set OSDEBUG=1

cmd /k title Build Window: %TARGET%/chk

