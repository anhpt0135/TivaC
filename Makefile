# Tiva Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
#######################################
# user configuration:
#######################################
# TARGET: name of the output file
TARGET = canbusOutput
# MCU: part number to build for
MCU = TM4C123GH6PM
# SOURCES: list of input source sources
SOURCES = main.c startup_gcc.c Timer_Config.c Adc_hal.c UART0.c Nokia5110.c Switch_led.c PLL.c
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -I. -I./src/
# OUTDIR: directory to use for output
OUTDIR = build
# TIVAWARE_PATH: path to tivaware folder
TIVAWARE_PATH = .

# LD_SCRIPT: linker script
#LD_SCRIPT = $(MCU).ld
LD_SCRIPT = TM4C123GH6PM_NODBG.ld


FLASHDIR = /AnhFolder/PROGRAMMING/ARM/Tm4c123_ANH/lm4tools/lm4flash

# define flags
CFLAGS = -g -O0 -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS = -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

CFLAGS +=-Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall

#CFLAGS += -Wno-variadic-macros -DPART_$(MCU) -c -I$(TIVAWARE_PATH)
CFLAGS += -pedantic -DPART_$(MCU) -c -I$(TIVAWARE_PATH)

CFLAGS += -DTARGET_IS_BLIZZARD_RA1 $(INCLUDES)
CFLAGS += -DNDEBUG

LDFLAGS = -L$(TIVAWARE_PATH)/driverlib/gcc -ldriver
LDFLAGS += -T $(LD_SCRIPT) --entry ResetISR --gc-sections


#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
RM      = rm -f
MKDIR	= mkdir -p
#######################################

LIBGCC:=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC:=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM:=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o)))

# default: build bin
all: $(OUTDIR)/$(TARGET).bin

$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/a.out: $(OBJECTS)
	#$(LD) -o $@ $^ $(LDFLAGS) '${LIBM}' '${LIBC}' '${LIBGCC}'
	$(LD) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/$(TARGET).bin: $(OUTDIR)/a.out
	$(OBJCOPY) -O binary $< $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)
	
flash:
	$(FLASHDIR)/lm4flash $(OUTDIR)/$(TARGET).bin

clean:
	$(RM) $(OUTDIR)/*

.PHONY: all clean
