######################################
# target
######################################
TARGET = lwip

TOP_DIR = $(shell pwd)


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build
SUB_DIRS = 2440bsp FreeRTOS-Kernel lwip-1.4.1

#######################################
# binaries
#######################################
PREFIX = arm-s3c2440-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
export CC = $(GCC_PATH)/$(PREFIX)gcc
export AS = $(GCC_PATH)/$(PREFIX)gcc
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
export CC = $(PREFIX)gcc
export AS = $(PREFIX)gcc
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=arm920t

# fpu
# NONE for Cortex-M0/M0+/M3
FPU =

# float-abi
FLOAT-ABI =

# mcu
MCU = $(CPU) $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =

# C includes
INCLUDE_DIRS = \
2440bsp/include \
lwip-1.4.1/src/include \
lwip-1.4.1/src/include/arch \
lwip-1.4.1/src/include/ipv4 \
FreeRTOS-Kernel/include \
FreeRTOS-Kernel/portable/GCC/ARM9_s3c2440 \
lwip-1.4.1/src/include \
lwip-1.4.1/src/include/arch \
lwip-1.4.1/src/include/ipv4 \

export C_INCLUDES = $(addprefix -I$(TOP_DIR)/,$(INCLUDE_DIRS))


# compile gcc flags
export ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

export CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -std=gnu99

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = lwip.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR = 

LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -nostartfiles

OBJ_FILES =

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: compile
# @echo "SUB_DIRS = $(SUB_DIRS)"
# @echo "$(foreach dir,$(SUB_DIRS),$(wildcard $(dir)/build/*))"
# TODO:这段话之前不能有任何语句否则$(OBJ_FILES)为空,原因未知,凑合用
	$(eval OBJ_FILES += $(foreach dir,$(SUB_DIRS),$(wildcard $(dir)/build/*.o*)))
# @echo "OBJ_FILES = $(OBJ_FILES)"
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $@
	$(SZ) $@
	$(PREFIX)objdump -D -m arm $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/$(TARGET).dis
# TODO:产生一个叫compile的文件,不知道原因,在这里删除
	$(foreach dir,$(SUB_DIRS),$(shell rm $(dir)/compile))

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

.PHONY: compile
compile:
	for dir in $(SUB_DIRS); do $(MAKE) -C $$dir; done

#######################################
# clean up
#######################################
.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR)
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(dir) clean;)


# $(eval OBJ_FILES += $(foreach dir,$(SUB_DIRS),$(wildcard $(dir)/build/*.o)))