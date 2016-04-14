# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
# 

OPT = 3

CORE = CORE_M3
# ARM CPU type (cortex-m0, cortex-m3)
CPU = cortex-m3
STMCHIP = STM32L1XX_MD
MCU = stm32l15xxB

HSE_VALUE=12000000

SILENCE = @

TARGETNAME = rpcFreeRTOSTemplate

SRC = src
# Libraries
LIBRARIES = $(SRC)/libraries

STD_PERIPHERAL_DRIVER = $(LIBRARIES)/STM32L1xx_StdPeriph_Lib_V1.3.1/Libraries/STM32L1xx_StdPeriph_Driver/
CMSIS = $(LIBRARIES)/STM32L1xx_StdPeriph_Lib_V1.3.1/Libraries/CMSIS

FREERTOSDIR = $(LIBRARIES)/FreeRTOSV8.2.1
FAT32DIR = $(LIBRARIES)/FatFs_R0.11/src

# Define all C source files (dependencies are generated automatically)
#

SOURCES += $(SRC)/main.c

SOURCES += $(SRC)/board.c
SOURCES += $(SRC)/task_led.c
SOURCES += $(SRC)/task_key.c
SOURCES += $(SRC)/task_adc.c
SOURCES += $(SRC)/task_rpc_serial_in.c
SOURCES += $(SRC)/syscalls.c
SOURCES += $(SRC)/lowpower.c




SOURCES += $(LIBRARIES)/serial.c
SOURCES += $(LIBRARIES)/startup/startup_stm32l1xx_md.s
SOURCES += $(LIBRARIES)/startup/system_stm32l1xx.c

#C_SCR += modules/rpc/src/client/app/rpc_network_freertos.c
SOURCES += modules/rpc/src/client/generated_app/RPC_TRANSMISSION_mcu2qt.c
SOURCES += modules/rpc/src/server/generated_app/RPC_TRANSMISSION_parser.c
SOURCES += modules/rpc/src/server/app/rpc_service_freertos.c
SOURCES += modules/rpc/src/server/app/rpc_func_freertos.c

 
SOURCES += modules/RPC-ChannelCodec/src/channel_codec/crc16.c
SOURCES += modules/RPC-ChannelCodec/src/channel_codec/channel_codec.c

ifeq (1,0)

SOURCES += $(FAT32DIR)/ff.c
endif

ifeq (1,1)
#free-rtos source code
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/tasks.c
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/queue.c
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/list.c
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/croutine.c
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c 
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/timers.c
#Memory management
SOURCES += $(FREERTOSDIR)/FreeRTOS/Source/portable/MemMang/heap_1.c
endif

#stm32l1xx library 
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/misc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_adc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_aes.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_aes_util.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_comp.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_crc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_dac.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_dbgmcu.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_dma.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_exti.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_flash.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_flash_ramfunc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_fsmc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_gpio.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_i2c.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_iwdg.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_lcd.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_opamp.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_pwr.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_rcc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_rtc.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_sdio.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_spi.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_syscfg.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_tim.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_usart.c
SOURCES += $(STD_PERIPHERAL_DRIVER)/src/stm32l1xx_wwdg.c
#SOURCES += $(LIBRARIES)/chip/stm32l1xx_clock_config.c

OBJECTS  = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SOURCES))))

DEPENDS  = $(addprefix $(OBJDIR)/,$(addsuffix .d,$(basename $(SOURCES))))

DEFS = -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=$(HSE_VALUE) -D$(STMCHIP) $(USER_DEFS)

# Object files directory
# Warning: this will be removed by make clean!
#
OBJDIR = buildarm

TARGET = $(OBJDIR)/$(TARGETNAME)

# Place -D, -U or -I options here for C and C++ sources
#CPPFLAGS += -ISource
CPPFLAGS += -Iinclude
CPPFLAGS += -Imodules/rpc/include
CPPFLAGS += -Imodules/RPC-ChannelCodec/include
CPPFLAGS += -Imodules/RPC-ChannelCodec/include/errorlogger_dummy

CPPFLAGS += -I$(FREERTOSDIR)/FreeRTOS/Source/include
CPPFLAGS += -I$(CMSIS)/Include
CPPFLAGS += -I$(CMSIS)/Device/ST/STM32L1xx/Include
CPPFLAGS += -I$(STD_PERIPHERAL_DRIVER)/inc
CPPFLAGS += -I$(FREERTOSDIR)/FreeRTOS/Source/portable/GCC/ARM_CM3/



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
CFLAGS += -DUSE_FULL_ASSERT
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
LDFLAGS += -T$(LIBRARIES)/startup/ldscripts/$(MCU).ld

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

SH = "C:\Program Files (x86)\Git\usr\bin\sh.exe"
MKDIR =  "C:\Program Files (x86)\Git\usr\bin\mkdir.exe"
TAIL =  "C:\Program Files (x86)\Git\usr\bin\tail.exe"

ifeq (AMD64, $(PROCESSOR_ARCHITEW6432))
  SUBWCREV = tools/SubWCRev64.exe
else
  SUBWCREV = tools/SubWCRev.exe
endif


# Compiler flags to generate dependency files
GENDEPFLAGS = -MMD -MP -MF $(OBJDIR)/$(*D)/$(*F).d



# Combine all necessary flags and optional flags
# Add target processor to flags.
#
CPU = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
#CPU = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
#CPU = -mcpu=cortex-m4 -mthumb 
#CPU = -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16

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
	rm -rf $(OBJDIR)
	rm -rf docs/html


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
