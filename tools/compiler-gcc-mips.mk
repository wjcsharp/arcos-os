OBJECT_SUFFIX=o
LIBRARY_SUFFIX=a

ifeq "$(TARGETTYPE)" "library"
TARGETEXT=$(LIBRARY_SUFFIX)
endif

ifeq "$(TARGETTYPE)" "executable"
TARGETEXT=
endif

#
# C compiler configuration
#
CC=gcc

ifneq "$(DEFINES)" ""
DEFINES:=$(addprefix -D,$(DEFINES))
endif

ifneq "$(INCLUDES)" ""
INCLUDES:=$(addprefix -I,$(INCLUDES))
endif

CFLAGS:=-ggdb -EL -G0 -mips32 -Wall
CPPFLAGS:=$(INCLUDES) $(DEFINES)

#
# Assembler configuration
#
AS:=$(CC)
ASFLAGS:=$(CFLAGS)

#
# Linker configuration
#
LD:=ld
LDFLAGS:=-Ttext 80020000 -EL -G0 -mips32

#
# Librarian configuration
#
AR:=ar
ARFLAGS:=-ruc
