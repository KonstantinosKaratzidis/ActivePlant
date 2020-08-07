# PROJDIR := $(realpath $(CURDIR)/..)

# prints the rules exactly as there are given in the cmd line
ifeq ($(ECHO), 1)
RECIPE_ECHO = 
else
RECIPE_ECHO = -@
endif

#ifeq (DEBUG_RULES, "1")
#RECIPE_ECHO = 
#else
#RECIPE_ECHO = -@
#endif

MCU = atmega328p
MCU_PLATFORM = avr
F_CPU = 16000000UL
BAUD = 9600UL # default to be used if not set in config.h
DEBUG_BAUD = 19200UL

AVRDUDE_PROG_TYPE = usbasp
AVRDUDE_PARTNO = m328p
AVRDUDE = avrdude -c $(AVRDUDE_PROG_TYPE) -p $(AVRDUDE_PARTNO)

# ---------------------------------------------
# source and object directory/files definitions
# ---------------------------------------------
SRCDIR := ./src
BUILDDIR := ./build
TARGET_NAME = protoplant
TARGET = $(BUILDDIR)/$(TARGET_NAME)

SUBDIRS =

ifeq ($(MCU_PLATFORM), avr)
SUBDIRS += avr
endif

SRCDIRS = $(SRCDIR) $(foreach dir, $(SUBDIRS), $(addprefix $(SRCDIR)/, $(dir)))
TARGETDIRS = $(BUILDDIR) $(foreach dir, $(SUBDIRS), $(addprefix $(BUILDDIR)/, $(dir)))

SOURCES_C := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
SOURCES_CXX := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
SOURCES = $(SOURCES_C) $(SOURCES_CXX)

OBJ_C := $(subst $(SRCDIR), $(BUILDDIR), $(SOURCES_C:.c=.o))
OBJ_CXX := $(subst $(SRCDIR), $(BUILDDIR), $(SOURCES_CXX:.cpp=.o))
OBJ = $(OBJ_C) $(OBJ_CXX)
DEPS = $(OBJ:.o=.d)

VPATH = $(SRCDIRS)

# ------------------------
# Compilation arguments
# ------------------------

CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

ARCH_FLAGS = -mmcu=$(MCU)
ARCH_CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -DDEBUG_BAUD=$(DEBUG_BAUD)
ARCH_LDFLAGS = -mmcu=$(MCU)
ARCH_CFLAGS = -mmcu=$(MCU)
ARCH_CXXFLAGS = -mmcu=$(MCU)

INCLUDES = $(foreach dir, $(SRCDIRS), $(addprefix -I, $(dir)))
CPPFLAGS = $(ARCH_CPPFLAGS) $(INCLUDES)

COMMON_FLAGS = -Os -g -Wall
COMMON_FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += $(ARCH_DEFINES)

CFLAGS = $(ARCH_CFLAGS) $(COMMON_FLAGS) -std=gnu99
CXXFLAGS = $(ARCH_CXXFLAGS) $(COMMON_FLAGS) -std=c++11

#CFLAGS = -Os -g -std=gnu99 -Wall
#CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,-Map,$(TARGET).map
LDFLAGS += -Wl,--gc-sections
LDFLAGS += $(ARCH_LDFLAGS)
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf

# ---------------------------------
#               Rules
# ---------------------------------

.PHONY: default all disassemble eeprom size clean build_dirs flash fuses debug

all: build_dirs $(TARGET).hex

-include $(DEPS)

define genRules
$(1)/%.o: %.c
	@echo CC $$< -\> $$@
	$(RECIPE_ECHO)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -o $$@ $$< -MMD

$(1)/%.o: %.cpp
	@echo CXX $$< -\> $$@
	$(RECIPE_ECHO)$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) -o $$@ $$< -MMD
endef

# generate rules for each target directory
$(foreach targetdir, $(TARGETDIRS), $(eval $(call genRules, $(targetdir))))

$(TARGET).elf: $(OBJ)
	@echo LD $@
	$(RECIPE_ECHO)$(CC) $(LDFLAGS) $^ -o $@

%.hex: %.elf
	@echo OBJCOPY \(code\) $< -\> $@
	$(RECIPE_ECHO)$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	@echo OBJCOPY \(eeprom\) $< -\> $@
	$(RECIPE_ECHO)$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

disassemble: $(TARGET).lst

size: $(TARGET).elf
	$(RECIPE_ECHO)$(SIZE) -C --mcu=$(MCU) $(TARGET).elf

build_dirs:
	$(RECIPE_ECHO)mkdir -pv $(TARGETDIRS) >/dev/null

clean:
	rm -rf $(BUILDDIR)

#
# Programmer
#

flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$<

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -U eeprom:w:$<

terminal:
	$(AVRDUDE) -n -t

#
# Fuse settings
#

## Mega 48, 88, 168, 328 default values
LFUSE = 0x62
HFUSE = 0xdf
EFUSE = 0x00

## Generic
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

fuses:
	$(AVRDUDE) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -nv

## Called with no extra definitions, sets to defaults
set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
set_default_fuses:  fuses

## Set the fuse byte for full-speed mode
## Note: can also be set in firmware for modern chips
set_fast_fuse: LFUSE = 0xE2
set_fast_fuse: FUSE_STRING = -U lfuse:w:$(LFUSE):m
set_fast_fuse: fuses

## Set the EESAVE fuse byte to preserve EEPROM across flashes
set_eeprom_save_fuse: HFUSE = 0xD7
set_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
set_eeprom_save_fuse: fuses

## Clear the EESAVE fuse byte
clear_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
clear_eeprom_save_fuse: fuses

# debug: print some constants, in order to see what is going on
debug:
	@echo ECHO = $(ECHO)
	@echo RECIPE_ECHO = $(RECIPE_ECHO)
	@echo
	@echo SOURCES_C = $(SOURCES_C)
	@echo SOURCES_CXX = $(SOURCES_CXX)
	@echo SOURCES = $(SOURCES)
	@echo
	@echo OBJ_C = $(OBJ_C)
	@echo OBJ_CXX = $(OBJ_CXX)
	@echo OBJ = $(OBJ)
	@echo
	@echo CFLAGS = $(CFLAGS)
	@echo CPPFLAGS = $(CPPFLAGS)

