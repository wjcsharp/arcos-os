#
# Determine build type
#
ifeq "$(OSDEBUG)" ""
FREEBUILD=1
else
FREEBUILD=0
endif

#
# Compilation host configuration
#
ifneq "$(WINDOWS_HOST)" ""
MKDIR=mkdir
RMDIR=rmdir /S /Q
SILENT=>nul 2>nul
DEL=del
/:=$(strip \)
else
MKDIR=mkdir -p
RMDIR=rm -rf
SILENT=>/dev/null
DEL=rm
/:=/
endif

#
# MIPS32 little-endian specific defines
#
ifdef MIPSEL32
TARGET_DIRECTORY=mipsel32
TARGET_DEFINES=MIPSEL32 MIPS32
endif # MIPSEL32

#
# Hosted Win32 defines
#
ifdef WIN32
TARGET_DIRECTORY=win32
TARGET_DEFINES=WIN32
endif # WIN32
