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
C_SOURCES =  \
main.c \
start.c \
usart.c \
nand.c \
irq.c \
exti.c \
dm9000.c \
timer.c 


# ASM sources
ASM_SOURCES =  \
start2.s

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