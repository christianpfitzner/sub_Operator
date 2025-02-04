#Makefile for OpenCM9.04 Board=======================================================================

#Configuration=======================================================================================

PROJECT_NAME	=	firmware

#Location of arm-none-eabi
#TCHAIN_PATH	=	/home/feesma44884/Programme/arm-2011.03-42-arm-none-eabi/bin/
TCHAIN_PATH	=	/home/jo/workspace/arm-2011.03-42-arm-none-eabi/bin/

#Use arm compiler from Robotis OpenCM Arduino Tool

#INCLUDES
INCLUDE_DIRS	=	-I. -I core/inc

#Librarys
LIBRARY_DIRS	=	-L. -L core/lib

#DEBUG
DEBUG_OPTION	=	-g
DEBUG_SYMBOL	=	-DCM9_DEBUG

#====================================================================================================

#Compiler Options====================================================================================

#Symbols
SYMBOLS		 =	-DBOARD_CM904 -DMCU_STM32F103CB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_FLASH

#Assembler
COMPILE_OPTS_ASM = -mcpu=cortex-m3 -march=armv7-m -mthumb $(SYMBOLS) $(DEBUG_SYMBOL)
COMPILE_OPTS_C	 = -Os $(DEBUG_OPTION) -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections -fdata-sections -Wl,--gc-sections $(SYMBOLS) $(DEBUG_SYMBOL)
COMPILE_OPTS_CPP = -Os $(DEBUG_OPTION) -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-rtti -fno-exceptions -Wall $(SYMBOLS) $(DEBUG_SYMBOL)

TCHAIN_PREFIX	= $(TCHAIN_PATH)arm-none-eabi-
#====================================================================================================

#Compiler settings===================================================================================
#Assembler
AS = $(TCHAIN_PREFIX)gcc
ASFLAGS = $(COMPILE_OPTS_ASM)

#C 
CC = $(TCHAIN_PREFIX)gcc
CFLAGS = $(COMPILE_OPTS_C) $(INCLUDE_DIRS)

#C++
CXX = $(TCHAIN_PREFIX)g++
CXXFLAGS = $(COMPILE_OPTS_CPP) $(INCLUDE_DIRS)
#====================================================================================================

#Linker Settings=====================================================================================

LD = $(TCHAIN_PREFIX)g++
LDFLAGS = -T /core/link/flash.ld $(LIBRARY_DIRS) -mcpu=cortex-m3 -mthumb -Xlinker --gc-sections -Wall

OBJCP = $(TCHAIN_PREFIX)objcopy
OBJCPFLAGS = -v -Obinary

OBJDUMP = $(TCHAIN_PREFIX)objdump
OBJDUMPFLAGS = -h -S -C -D
#Archive maintaining program -> ar archiver
AR = $(TCHAIN_PREFIX)ar
ARFLAGS = cr

MAIN_OUT     = $(PROJECT_NAME)
MAIN_OUT_ELF = $(MAIN_OUT).elf
MAIN_OUT_BIN = $(MAIN_OUT).bin
MAIN_OUT_LSS = $(MAIN_OUT).lss
#====================================================================================================

#Make all============================================================================================

all: $(MAIN_OUT_ELF) $(MAIN_OUT_BIN) 

MAIN_OBJS += core/src/exc.o\
 core/src/descriptors.o \
 core/src/adc.o \
 core/src/exti.o \
 core/src/flash.o \
 core/src/gpio.o \
 core/src/i2c.o \
 core/src/iwdg.o \
 core/src/nvic.o \
 core/src/pwr.o \
 core/src/rcc.o \
 core/src/spi.o \
 core/src/syscalls.o \
 core/src/systick.o \
 core/src/timer.o \
 core/src/usart.o \
 core/src/usb.o \
 core/src/usb_callbacks.o \
 core/src/usb_core.o \
 core/src/usb_hardware.o \
 core/src/usb_init.o \
 core/src/usb_int.o \
 core/src/usb_mem.o \
 core/src/usb_regs.o \
 core/src/boards.o \
 core/src/cxxabi-compat.o \
 core/src/ext_interrupts.o \
 core/src/HardwareSerial.o \
 core/src/HardwareSPI.o \
 core/src/HardwareTimer.o \
 core/src/Print.o \
 core/src/pwm.o \
 core/src/usb_serial.o \
 src/$(PROJECT_NAME).o \
 core/src/CM900.o \
 core/src/CM904.o \
 core/src/Dynamixel.o \
 core/src/dxl.o \
 core/src/Arduino-compatibles.o
 
$(MAIN_OUT_ELF): $(MAIN_OBJS) 
	$(LD) $(LDFLAGS) $(MAIN_OBJS) --output $@

$(MAIN_OUT_BIN): $(MAIN_OUT_ELF)
	$(OBJCP) $(OBJCPFLAGS) $< $@

#$(MAIN_OUT_LSS): $(MAIN_OUT_ELF)
#	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
#====================================================================================================

#Make Clean==========================================================================================

clean :
	-rm -v src/*.o core/src/*.o $(MAIN_OUT_ELF)
#====================================================================================================

#Load user program===================================================================================
flash :
	./core/robotis-loader/robotis-loader.py /dev/opencm_robotarm firmware.bin
#====================================================================================================


