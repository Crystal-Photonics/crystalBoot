# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
# 

OPT = s

SILENCE = @
#SILENCE = 
TARGETNAME = rpcFreeRTOSTemplate

SRC = src


PORT_DIR = $(SRC)/port/stm32_l1
include $(PORT_DIR)/makefile.port

BOOTLOADER_CONFIG_DIR = bootloaderconfig

PORT_SOURCES += $(BOOTLOADER_CONFIG_DIR)/board/port_board.c
PORT_SOURCES += $(BOOTLOADER_CONFIG_DIR)/board/port_board_preinit.c
PORT_SOURCES += $(BOOTLOADER_CONFIG_DIR)/key/aes_128_key.c

PORT_CPPFLAGS += -I$(BOOTLOADER_CONFIG_DIR)/board
PORT_CPPFLAGS += -I$(BOOTLOADER_CONFIG_DIR)/key

SOURCES += $(SRC)/main.c
SOURCES += $(SRC)/syscalls.c
SOURCES += $(SRC)/rpc_receiver.c
SOURCES += $(SRC)/programmer.c
SOURCES += modules/rpc/src/client/generated_app/RPC_TRANSMISSION_mcu2qt.c
SOURCES += modules/rpc/src/server/generated_app/RPC_TRANSMISSION_parser.c
SOURCES += modules/rpc/src/server/app/rpc_service_freertos.c
SOURCES += modules/rpc/src/server/app/rpc_func_freertos.c
SOURCES += modules/RPC-ChannelCodec/src/channel_codec/crc16.c
SOURCES += modules/RPC-ChannelCodec/src/channel_codec/channel_codec.c
SOURCES += modules/crypto-algorithms/sha256.c
SOURCES += modules/tiny-AES128-C/aes.c

SOURCES += $(PORT_SOURCES)

OBJECTS  = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SOURCES))))

DEPENDS  = $(addprefix $(OBJDIR)/,$(addsuffix .d,$(basename $(SOURCES))))

DEFS = -DHSE_VALUE=$(HSE_VALUE)
DEFS += -DECB=0
DEFS +=-D$(MCU)
DEFS += $(PORT_DEFS)

# Object files directory
# Warning: this will be removed by make clean!
#
OBJDIR = buildarm

TARGET = $(OBJDIR)/$(TARGETNAME)

# Place -D, -U or -I options here for C and C++ sources

CPPFLAGS += -Iinclude
CPPFLAGS += -Imodules/rpc/include
CPPFLAGS += -Imodules/RPC-ChannelCodec/include
CPPFLAGS += -Imodules/RPC-ChannelCodec/include/errorlogger_dummy
CPPFLAGS += -Imodules/crypto-algorithms
CPPFLAGS += -Imodules/tiny-AES128-C
CPPFLAGS += $(PORT_CPPFLAGS)

#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = $(DEFS)
CFLAGS += -mthumb
CFLAGS += -O$(OPT) 
CFLAGS += -std=gnu99
CFLAGS += -gdwarf-2
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unknown-pragmas
#CFLAGS += -flto
CFLAGS += -Wextra
CFLAGS += -Wpedantic
#CFLAGS += -Wpointer-arith
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Winline
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wundef
CFLAGS += -Wa,-adhlns=$(OBJDIR)/$(*D)/$(*F).lst

# Optimize use of the single-precision FPU
#
#CFLAGS += -fsingle-precision-constant

# use USE_FULL_ASSERT
#CFLAGS += -DUSE_FULL_ASSERT

# This will not work without recompiling App/STM32F4-Discovery/Libraries
#
# CFLAGS += -fshort-double

#---------------- Compiler Options C++ ----------------
#
CXXFLAGS  = $(CFLAGS)

#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler
#  -adhlns:   create listing
#
ASFLAGS = -Wa,-adhlns=$(OBJDIR)/$(*D)/$(*F).lst




#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS += -lm
LDFLAGS += --specs=nano.specs
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += $(PORT_LDFLAGS)

#============================================================================


# Define programs and commands
#TOOLCHAIN = arm-none-eabi
TOOLCHAIN = C:/arm/gcc-4.9.3-20150609/bin/arm-none-eabi-

CC        = $(TOOLCHAIN)gcc
OBJCOPY   = $(TOOLCHAIN)objcopy
OBJDUMP   = $(TOOLCHAIN)objdump
SIZE      = $(TOOLCHAIN)size
NM        = $(TOOLCHAIN)nm
GDB 	= $(TOOLCHAIN)gdb
STRIP 	= $(TOOLCHAIN)strip
LD 		= $(TOOLCHAIN)ld
OPENOCD   = openocd
DOXYGEN   = doxygen
STLINK    = st-flash

SH 			= "C:\Program Files\Git\usr\bin\sh.exe"
MKDIR 		= "C:\Program Files\Git\usr\bin\mkdir.exe"
TAIL		= "C:\Program Files\Git\usr\bin\tail.exe"

ifeq (AMD64, $(PROCESSOR_ARCHITEW6432))
  SUBWCREV = tools/SubWCRev64.exe
else
  SUBWCREV = tools/SubWCRev.exe
endif


# Compiler flags to generate dependency files
GENDEPFLAGS = -MMD -MP -MF $(OBJDIR)/$(*D)/$(*F).d





CFLAGS   += $(CPU)
CXXFLAGS += $(CPU)
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU)

# Default target.
all:  gccversion writegitversion build showsize

build: elf hex lss sym bin

elf: $(TARGET).elf
hex: $(TARGET).hex
bin: $(TARGET).bin
lss: $(TARGET).lss
sym: $(TARGET).sym


doxygen:
#	@echo
	#@echo Creating Doxygen documentation
#	@$(DOXYGEN)

# Display compiler version information
gccversion: 
	@$(CC) --version


# Show the final program size
showsize: elf
#	@echo 
#	$(NM) --print-size --size-sort --reverse-sort $(TARGET).elf
	$(SIZE) $(TARGET).elf


program: all
	flash_chip.bat $(BUILDDIR)"$(TARGET)".bin


# Target: clean project
clean:
	@echo Cleaning project:
	rm  -rf $(OBJDIR)
	


# Create extended listing file from ELF output file
%.lss: %.elf
#	@echo
	@echo Creating Extended Listing: $@
	$(SILENCE)$(OBJDUMP) -h -S -z $< > $@


# Create a symbol table from ELF output file
%.sym: %.elf
#	@echo
	@echo Creating Symbol Table: $@
	$(SILENCE)$(NM) -n $< > $@

writegitversion:
	@echo writing vc.h..
	$(SILENCE)$(SH) git.sh 
	
# Link: create ELF output file from object files
.SECONDARY: $(TARGET).elf
.PRECIOUS:  $(OBJECTS)
$(TARGET).elf: $(OBJECTS)
#	@echo
	@echo Linking: $@
	$(SILENCE)$(CC) $^ $(LDFLAGS) --output $@ 


# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
#	@echo
	@echo Creating hex file: $@
	$(SILENCE)$(OBJCOPY) -O ihex $< $@

# Create bin file :
%.bin: %.elf
#	@echo
	@echo Creating bin file: $@
	$(SILENCE)$(OBJCOPY) -O binary $< $@


# Compile: create object files from C source files
$(OBJDIR)/%.o : %.c
#	@echo
#	$MKDIR
	@echo Compiling C: $<
#echo $(DEPENDS)
	$(SILENCE)$(MKDIR) -p $(@D)
	$(SILENCE)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(GENDEPFLAGS) $< -o $@ 


# Compile: create object files from C++ source files
$(OBJDIR)/%.o : %.cpp
#	@echo
	@echo Compiling CPP: $<
	$(SILENCE)$(MKDIR) -p $(@D)
	$(SILENCE)$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(GENDEPFLAGS) $< -o $@ 


# Assemble: create object files from assembler source files
$(OBJDIR)/%.o : %.s
#	@echo 
	@echo Assembling: $<
	$(SILENCE)$(MKDIR) -p $(@D)
	$(SILENCE)$(CC) -c $(CPPFLAGS) $(ASFLAGS) $< -o $@

# Include the dependency files
-include $(DEPENDS)


# Listing of phony targets
.PHONY: all build flash clean \
        doxygen elf lss sym \
        showsize gccversion
