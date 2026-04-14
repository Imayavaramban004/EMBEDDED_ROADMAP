# =============================================================================
# STM32F4xx bare-metal build system
# Toolchain : arm-none-eabi-gcc (GNU Arm Embedded Toolchain 12+)
# Standard  : C11 (gnu11 dialect for __attribute__ and asm extensions)
# Usage     : make          → build debug binary
#             make RELEASE=1 → build release binary (optimised, no debug info)
#             make flash     → flash via OpenOCD (ST-Link)
#             make clean     → remove build artefacts
#             make help      → print target summary
# =============================================================================

# -----------------------------------------------------------------------------
# Guard: abort immediately if invoked with a non-GNU make
# -----------------------------------------------------------------------------
ifeq (,$(findstring GNU Make,$(shell $(MAKE) --version 2>&1)))
  $(error This Makefile requires GNU Make. Found: $(MAKE))
endif

# -----------------------------------------------------------------------------
# Project identity
# -----------------------------------------------------------------------------
TARGET      := firmware
# Output basename  → firmware.elf / .bin / .hex
MCU_FAMILY  := STM32F446xx       # Used in -D flag and error messages

# -----------------------------------------------------------------------------
# Toolchain
# All tools share the same prefix so swapping to a different sysroot only
# requires changing one variable.
# -----------------------------------------------------------------------------
CROSS       := arm-none-eabi
CC          := $(CROSS)-gcc
AS          := $(CROSS)-as
LD          := $(CROSS)-gcc      # Use GCC as the linker driver (handles crt0, libgcc)
OBJCOPY     := $(CROSS)-objcopy
OBJDUMP     := $(CROSS)-objdump
SIZE        := $(CROSS)-size
GDB         := $(CROSS)-gdb

# -----------------------------------------------------------------------------
# CPU / ABI flags — shared between compiler and linker
# -----------------------------------------------------------------------------
CPU_FLAGS   := -mcpu=cortex-m4 \
               -mthumb \
               -mfpu=fpv4-sp-d16 \
               -mfloat-abi=hard   # Use FPU registers; change to 'soft' if no FPU

# -----------------------------------------------------------------------------
# Source and include layout
# -----------------------------------------------------------------------------
SRC_DIR     := src
INC_DIR     := inc
BUILD_DIR   := build
STARTUP_DIR := startup

# Collect all .c files — src/ and startup/ kept separate so patsubst
# works correctly for both directories.  Merging them into one SRCS and
# using a single patsubst leaves startup/*.c unchanged (no match), which
# then lands in DEPS and causes Make to -include a .c file → "missing
# separator" error.
SRC_SRCS     := $(wildcard $(SRC_DIR)/*.c)
STARTUP_SRCS := $(wildcard $(STARTUP_DIR)/*.c)

# Object files — each source maps to BUILD_DIR/<basename>.o
SRC_OBJS     := $(patsubst $(SRC_DIR)/%.c,     $(BUILD_DIR)/%.o, $(SRC_SRCS))
STARTUP_OBJS := $(patsubst $(STARTUP_DIR)/%.c, $(BUILD_DIR)/%.o, $(STARTUP_SRCS))
OBJS         := $(SRC_OBJS) $(STARTUP_OBJS)
DEPS         := $(OBJS:.o=.d)

# Linker script
LDSCRIPT    := linker/stm32f4.ld

# -----------------------------------------------------------------------------
# Compiler flags
# -MMD -MP   : generate .d dependency files alongside .o files
# -Wall …    : catch common bugs at compile time
# -fdata-sections / -ffunction-sections : enable dead-code elimination (--gc-sections)
# -ffreestanding : tell the compiler there is no hosted runtime (no libc assumed)
# -----------------------------------------------------------------------------
COMMON_FLAGS := $(CPU_FLAGS)       \
                -std=gnu11         \
                -Wall              \
                -Wextra            \
                -Wshadow           \
                -Wdouble-promotion \
                -Wformat=2         \
                -fdata-sections    \
                -ffunction-sections \
                -ffreestanding     \
                -MMD -MP

# Debug vs release profiles
# Override on the command line:  make RELEASE=1
ifdef RELEASE
  OPT_FLAGS := -O2 -DNDEBUG
  BUILD_DIR := build/release
else
  OPT_FLAGS := -O0 -g3 -gdwarf-4   # -g3 includes macro definitions in DWARF
  BUILD_DIR := build/debug
  COMMON_FLAGS += -DDEBUG
endif

CFLAGS      := $(COMMON_FLAGS) $(OPT_FLAGS) -I$(INC_DIR) -D$(MCU_FAMILY)

# Recalculate OBJS/DEPS after BUILD_DIR may have been overridden by RELEASE
SRC_OBJS     := $(patsubst $(SRC_DIR)/%.c,     $(BUILD_DIR)/%.o, $(SRC_SRCS))
STARTUP_OBJS := $(patsubst $(STARTUP_DIR)/%.c, $(BUILD_DIR)/%.o, $(STARTUP_SRCS))
OBJS         := $(SRC_OBJS) $(STARTUP_OBJS)
DEPS         := $(OBJS:.o=.d)

# -----------------------------------------------------------------------------
# Linker flags
# -T            : linker script
# --gc-sections : discard unused input sections (requires -ffunction-sections)
# --print-memory-usage : print region utilisation after linking
# -Map          : emit a map file for post-mortem analysis
# --cref        : add cross-reference table to the map file
# Spec files:
#   --specs=nosys.specs  : stub out syscalls (write, _sbrk, etc.) — no semihosting
#   --specs=nano.specs   : use newlib-nano (smaller printf/malloc footprint)
# -----------------------------------------------------------------------------
LDFLAGS     := $(CPU_FLAGS)                          \
               -T $(LDSCRIPT)                        \
               -Wl,--gc-sections                     \
               -Wl,--print-memory-usage              \
               -Wl,-Map=$(BUILD_DIR)/$(TARGET).map   \
               -Wl,--cref                            \
               -Wl,--no-warn-rwx-segments            \
               --specs=nosys.specs                   \
               --specs=nano.specs                    \
               -nostartfiles                         \
               -lc -lm                               # Link against newlib and libm

# -----------------------------------------------------------------------------
# OpenOCD / flash configuration
# -----------------------------------------------------------------------------
OPENOCD     := openocd
OCD_CFG     := -f interface/stlink.cfg -f target/stm32f4x.cfg
OCD_FLASH   := -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# =============================================================================
# Build rules
# =============================================================================

# Default goal — build the ELF and derive binary artefacts
.PHONY: all
all: $(BUILD_DIR)/$(TARGET).elf \
     $(BUILD_DIR)/$(TARGET).bin \
     $(BUILD_DIR)/$(TARGET).hex \
     $(BUILD_DIR)/$(TARGET).lss
	@echo ""
	@$(SIZE) --format=berkeley $(BUILD_DIR)/$(TARGET).elf

# -----------------------------------------------------------------------------
# Link
# -----------------------------------------------------------------------------
$(BUILD_DIR)/$(TARGET).elf: $(OBJS) $(LDSCRIPT) | $(BUILD_DIR)
	@echo "[LD]  $@"
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# -----------------------------------------------------------------------------
# Compile — pattern rules for src/ and startup/ directories.
# Two separate rules are required because patsubst needs a distinct stem
# for each source directory.  A single rule with VPATH would hide which
# directory a file came from and makes dependency tracking fragile.
# The | (order-only) prerequisite creates the output directory if missing
# without triggering a rebuild when only the timestamp changes.
# -----------------------------------------------------------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "[CC]  $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for startup files (startup/startupstm32f4.c → build/.../startupstm32f4.o)
$(BUILD_DIR)/%.o: $(STARTUP_DIR)/%.c | $(BUILD_DIR)
	@echo "[AS]  $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------
# Post-processing artefacts
# -----------------------------------------------------------------------------

# Raw binary — used by most flash loaders and DFU utilities
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@echo "[BIN] $@"
	$(OBJCOPY) -O binary $< $@

# Intel HEX — used by STM32CubeProgrammer, J-Flash, etc.
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@echo "[HEX] $@"
	$(OBJCOPY) -O ihex $< $@

# Annotated disassembly — invaluable for debugging HardFaults
# Interleaves source lines (-S), all sections (-D), and demangles C++ names
$(BUILD_DIR)/$(TARGET).lss: $(BUILD_DIR)/$(TARGET).elf
	@echo "[LSS] $@"
	$(OBJDUMP) -D -S --demangle $< > $@

# -----------------------------------------------------------------------------
# Directory creation
# -----------------------------------------------------------------------------
$(BUILD_DIR):
	@mkdir -p $@

# -----------------------------------------------------------------------------
# Dependency inclusion
# -include silently skips missing .d files on the first build
# -----------------------------------------------------------------------------
-include $(DEPS)

# =============================================================================
# Utility targets
# =============================================================================

# Flash via OpenOCD + ST-Link
.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).elf
	@echo "[FLASH] $<"
	$(OPENOCD) $(OCD_CFG) $(OCD_FLASH)

# Start a GDB session (requires OpenOCD running in another terminal)
.PHONY: debug
debug: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) -ex "target extended-remote :3333" \
	       -ex "monitor reset halt"           \
	       -ex "load"                         \
	       $<

# Print section sizes in Berkeley format
.PHONY: size
size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) --format=berkeley $<

# Remove build artefacts — works on Linux, macOS, and Git Bash on Windows
.PHONY: clean
clean:
	@echo "[CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

# Remove all build profiles
.PHONY: distclean
distclean:
	@echo "[DISTCLEAN] build/"
	@rm -rf build/

# Verify the toolchain is reachable before attempting a build
.PHONY: check-tools
check-tools:
	@which $(CC)       > /dev/null 2>&1 || (echo "ERROR: $(CC) not found"; exit 1)
	@which $(OBJCOPY)  > /dev/null 2>&1 || (echo "ERROR: $(OBJCOPY) not found"; exit 1)
	@which $(OPENOCD)  > /dev/null 2>&1 || (echo "ERROR: $(OPENOCD) not found"; exit 1)
	@echo "Toolchain OK: $$($(CC) --version | head -1)"

# Print all variable values — useful when debugging the build system itself
.PHONY: print-%
print-%:
	@echo '$* = $($*)'

# Help text
.PHONY: help
help:
	@echo "┌─────────────────────────────────────────────────────────┐"
	@echo "│  $(TARGET) build system                                  │"
	@echo "├──────────────────┬──────────────────────────────────────┤"
	@echo "│ make             │ Debug build  (O0, DWARF-4)           │"
	@echo "│ make RELEASE=1   │ Release build (O2, no debug info)    │"
	@echo "│ make flash       │ Flash via OpenOCD + ST-Link          │"
	@echo "│ make debug       │ Launch arm-none-eabi-gdb             │"
	@echo "│ make size        │ Print section sizes                  │"
	@echo "│ make clean       │ Remove current profile build dir     │"
	@echo "│ make distclean   │ Remove all build dirs                │"
	@echo "│ make check-tools │ Verify toolchain availability        │"
	@echo "│ make print-VAR   │ Print value of any Makefile variable │"
	@echo "└──────────────────┴──────────────────────────────────────┘"