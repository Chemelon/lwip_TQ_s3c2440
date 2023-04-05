#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
SRC_DIR = .

# C sources
# TODO: croutine.c 和 stream_buffer.c 不知道干什么用的,先不添加
C_SOURCES = \
event_groups.c \
list.c \
queue.c \
tasks.c \
timers.c \
\
portable/GCC/ARM9_s3c2440/port.c \
portable/GCC/ARM9_s3c2440/portISR.c \
\
portable/MemMang/heap_4.c


# ASM sources
ASM_SOURCES =

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(OBJECTS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR)