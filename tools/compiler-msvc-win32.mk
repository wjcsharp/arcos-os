OBJECT_SUFFIX=obj

ifeq "$(TARGETTYPE)" "library"
TARGETEXT=lib
endif

ifeq "$(TARGETTYPE)" "executable"
TARGETEXT=exe
endif

#
# C compiler configuration
#
CC=cl

ifneq "$(DEFINES)" ""
DEFINES:=$(addprefix /D,$(DEFINES))
endif

ifneq "$(INCLUDES)" ""
INCLUDES:=$(subst /,$/,$(INCLUDES))
INCLUDES:=$(addprefix /I,$(INCLUDES))
endif

CFLAGS:=/Zi /RTCu /nologo /Fd$(OBJECT_DIRECTORY)/
CPPFLAGS:=$(INCLUDES) $(DEFINES)

#
# Assembler configuration
#
AS:=$(CC)
ASFLAGS:=$(CFLAGS)

#
# Linker configuration
#
LD:=link
LDFLAGS:=/subsystem:console /nologo /debug

#
# Librarian configuration
#

