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
src/core/def.c \
src/core/dhcp.c \
src/core/dns.c \
src/core/init.c \
src/core/mem.c \
src/core/memp.c \
src/core/netif.c \
src/core/pbuf.c \
src/core/raw.c \
src/core/stats.c \
src/core/sys.c \
src/core/tcp_in.c \
src/core/tcp_out.c \
src/core/tcp.c \
src/core/timers.c \
src/core/udp.c \
src/core/ipv4/autoip.c \
src/core/ipv4/icmp.c \
src/core/ipv4/igmp.c \
src/core/ipv4/inet_chksum.c \
src/core/ipv4/inet.c \
src/core/ipv4/ip_addr.c \
src/core/ipv4/ip_frag.c \
src/core/ipv4/ip.c \
src/netif/etharp.c \
src/netif/ethernetif.c \
src/netif/slipif.c


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