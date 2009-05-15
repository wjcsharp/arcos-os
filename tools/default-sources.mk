#
# Select build target and platform specific defines
#
include $(TOOLSDIR)/platform.mk

#
# Set default directory for object files
#
_OBJ_DIR:=obj
_LIB_DIR:=$(BASEDIR)/build/lib

#
# Define a simple macro for use in ./sources and ./Makefile.inc
#
#O:=$(_OBJ_DIR)/$(TARGET_DIRECTORY)

#
# Include developer supplied ./sources file
#
include ./sources

#
# Try to include developer supplied sources file from target directory
#
-include ./$(TARGET_DIRECTORY)/sources

#
# Check if SOURCES macro is defined
#
ifndef SOURCES
$(error Your .$/sources file must define the SOURCES= macro.)
endif

#
# Check if TARGETPATH macro is defined
#
ifndef TARGETPATH
$(error Your .$/sources file must define the TARGETPATH= macro.)
endif

#
# Check if TARGETTYPE macro is defined
#
ifndef TARGETTYPE
$(error Your .$/sources file must define the TARGETTYPE= macro.)
endif

#
# Check if TARGETNAME macro is defined
#
ifndef TARGETNAME
$(error Your .$/sources file must define the TARGETNAME= macro.)
endif

#
# If TARGETPATH is "obj", we set TARGETPATH to default object directory
#
ifeq "$(TARGETPATH)" "obj"
TARGETPATH:=$(_OBJ_DIR)
endif

#
# If TARGETPATH is "lib", we set TARGETPATH to default library directory
#
ifeq "$(TARGETPATH)" "lib"
TARGETPATH:=$(_LIB_DIR)
endif


#
# Checked/free build specific macros
#
ifeq "$(FREEBUILD)" "1"
DEFINES=NDEBUG
else
DEFINES=DEBUG
endif # $(FREEBUILD)

#
# Append target-specific defines
#
DEFINES:=$(DEFINES) $(TARGET_DEFINES)

#
# Enable kernel debugger
#
ifeq "$(KD)" "1"
DEFINES:=$(DEFINES) HAVE_KD
endif

#
# Add main include directory
#
INCLUDES+=$(BASEDIR)/include

#
# Replace wildcards in include & library paths
#
LIBS:=$(subst *,$(TARGET_DIRECTORY),$(LIBS))
INCLUDES:=$(subst *,$(TARGET_DIRECTORY),$(INCLUDES))

#
# Build configuration-specific object path
#
OBJECT_DIRECTORY:=$(_OBJ_DIR)/$(TARGET_DIRECTORY)

#
# Macro to translate POSIX path name to platform-specific path name
# (replaces / with \ when needed)
#
# $(call MAKE_NATIVE_PATH,posix-path)
MAKE_NATIVE_PATH=$(subst /,$/,$(1))

#
# Select compiler-specific defines
#
ifeq "$(TARGET_DIRECTORY)" "mipsel32"
include $(TOOLSDIR)/compiler-gcc-mips.mk
endif
ifeq "$(TARGET_DIRECTORY)" "win32"
include $(TOOLSDIR)/compiler-msvc-win32.mk
endif

#
# Append compiler-specific library name suffix
#
LIBS:=$(addsuffix .$(LIBRARY_SUFFIX),$(LIBS))

#
# Create target file name
#
TARGET:=$(TARGETPATH)/$(TARGET_DIRECTORY)/$(TARGETNAME)
ifneq "$(TARGETEXT)" ""
TARGET:=$(TARGET).$(TARGETEXT)
endif

#
# Create a list of objects to build
#
OBJECTS:=$(notdir $(SOURCES))
OBJECTS:=$(OBJECTS:.c=.$(OBJECT_SUFFIX))
OBJECTS:=$(OBJECTS:.S=.$(OBJECT_SUFFIX))

#
# Set up file search paths
#
vpath %.c $(TARGET_DIRECTORY)
vpath %.h $(TARGET_DIRECTORY)
vpath %.S $(TARGET_DIRECTORY)
vpath %.$(OBJECT_SUFFIX) $(OBJECT_DIRECTORY)
vpath %.d $(OBJECT_DIRECTORY)

#
# Do not print executed commands (comment out when debugging makefiles)
#
.SILENT :

#
# Default target rule
#
.PHONY: all
all : $(TARGETPATH)/$(TARGET_DIRECTORY) $(OBJECT_DIRECTORY) $(TARGET)

#
# Rule to create directory for object files
#
$(OBJECT_DIRECTORY) :
	$(MKDIR) $(call MAKE_NATIVE_PATH,$@)

#
# Rule to create directory for target file
# (neccessary only if different from $(OBJECT_DIRECTORY)
#
ifneq "$(OBJECT_DIRECTORY)" "$(TARGETPATH)/$(TARGET_DIRECTORY)"
$(TARGETPATH)/$(TARGET_DIRECTORY) :
	$(MKDIR) $(call MAKE_NATIVE_PATH,$@)
endif

#
# Rule to link target
#
ifeq "$(COMPILER)" "gcc"

# GCC, create executable
ifeq "$(TARGETTYPE)" "executable"
$(TARGET) : $(OBJECTS) $(LIBS)
	@echo === Linking $(@F) for $(TARGET_DIRECTORY)...
	$(LD) $(LDFLAGS) -o $@ $(addprefix $(OBJECT_DIRECTORY)/,$(notdir $(OBJECTS))) $(LIBS)
endif

# GCC, create library
ifeq "$(TARGETTYPE)" "library"
$(TARGET) : $(OBJECTS)
	@echo === Building library $(@F) for $(TARGET_DIRECTORY)...
	$(AR) $(ARFLAGS) $@ $(addprefix $(OBJECT_DIRECTORY)/,$(notdir $^))
endif
	
else # MSVC

# MSVC, create executable
ifeq "$(TARGETTYPE)" "executable"
$(TARGET) : $(OBJECTS) $(LIBS)
	@echo === Linking $(@F) for $(TARGET_DIRECTORY)...
	$(LD) $(LDFLAGS) /out:$@ $(addprefix $(OBJECT_DIRECTORY)/,$(notdir $(OBJECTS))) $(LIBS)
endif

# MSVC, create library
ifeq "$(TARGETTYPE)" "library"
$(TARGET) : $(OBJECTS)
	@echo === Building library $(@F) for $(TARGET_DIRECTORY)...
	$(AR) $(ARFLAGS) /out:$@ $(addprefix $(OBJECT_DIRECTORY)/,$(notdir $^))
endif

endif # MSVC

#
# Rule to compile a C source file
#
ifeq "$(COMPILER)" "gcc"
%.o : %.c
	@echo === Compilig $< for $(TARGET_DIRECTORY)...
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $(OBJECT_DIRECTORY)/$(@F)
else # MSVC
%.obj : %.c
	@echo === Compilig $< for $(TARGET_DIRECTORY)...
	$(CC) /c $(CFLAGS) $(CPPFLAGS) $< /Fo$(OBJECT_DIRECTORY)/$(@F)
endif

#
# Rule to compile an Assembler source file
#
%.o : %.S
	@echo === Compilig $< for $(TARGET_DIRECTORY)...
	$(AS) -c $(ASFLAGS) $(CPPFLAGS) $< -o $(OBJECT_DIRECTORY)/$(@F)

#
# How to create dependency file from source file
#
# BUGBUG: the MKDIR command should be removed somehow.
# Without the mkdir command, the first attempt to create dependencies
# after "make clean" fails, because the directory does not exist :(
#MAKEDEPEND=$(MKDIR) $(call MAKE_NATIVE_PATH,$(OBJECT_DIRECTORY)) $(SILENT) & \
#    $(CC) -M $(CPPFLAGS) $< | sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" > $(OBJECT_DIRECTORY)/$(@F)

#
# Rule to create C dependecy file
#
$(OBJECT_DIRECTORY)/%.d : %.c
	-$(MKDIR) $(call MAKE_NATIVE_PATH,$(OBJECT_DIRECTORY)) $(SILENT)
	$(CC) -M $(CPPFLAGS) $< | sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" > $(OBJECT_DIRECTORY)/$(@F)

#
# Rule to create assembler dependecy file
#
$(OBJECT_DIRECTORY)/%.d : %.S
	-$(MKDIR) $(call MAKE_NATIVE_PATH,$(OBJECT_DIRECTORY)) $(SILENT)
	$(CC) -M $(CPPFLAGS) $< | sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" > $(OBJECT_DIRECTORY)/$(@F)

#
# Rule to clean up
#
.PHONY : clean
clean :
	-$(DEL) $(call MAKE_NATIVE_PATH, $(TARGET))
	-$(RMDIR) $(call MAKE_NATIVE_PATH, $(OBJECT_DIRECTORY))

#
# Include dependency files, but not when when cleaning up
#
ifneq "$(MAKECMDGOALS)" "clean"
DEPENDENCIES:=$(SOURCES:.c=.d)
DEPENDENCIES:=$(DEPENDENCIES:.S=.d)
ifeq "$(COMPILER)" "gcc"
-include $(addprefix $(OBJECT_DIRECTORY)/,$(DEPENDENCIES))
endif
endif
