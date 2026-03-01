NAME     := $(notdir $(CURDIR))
CC       := riscv32-none-elf-gcc
CXX      := riscv32-none-elf-g++
OBJCOPY  := riscv32-none-elf-objcopy
AS       := riscv32-none-elf-as
CFLAGS   := -Wall -Wextra -Werror -Wpedantic
CXXFLAGS := -Wall -Wextra -Werror -Wpedantic -fno-exceptions -fno-rtti
LDFLAGS  := -s -T linker.ld -nostdlib -nostartfiles -nodefaultlibs

OUT_DIR  := $(BUILD_DIR)/$(NAME)
TARGET   := $(BUILD_DIR)/$(NAME).bin

C_SRCS   := $(wildcard *.c)
CXX_SRCS := $(wildcard *.cpp)
ASM_SRCS := $(wildcard *.asm)
HEADERS  := $(wildcard *.h *.hpp)
LSCRIPTS := $(wildcard *.ld)
OBJS     := $(patsubst %.c,$(OUT_DIR)/%.o,$(C_SRCS)) \
            $(patsubst %.cpp,$(OUT_DIR)/%.o,$(CXX_SRCS)) \
            $(patsubst %.asm,$(OUT_DIR)/%.o,$(ASM_SRCS))

LINKER   := $(if $(CXX_SRCS),$(CXX),$(CC))

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -o $(patsubst %.bin,%.elf,$@) $^
	$(OBJCOPY) -O binary $(patsubst %.bin,%.elf,$@) $@
	rm $(patsubst %.bin,%.elf,$@)

$(OUT_DIR)/%.o: %.c $(HEADERS) $(LSCRIPTS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/%.o: %.cpp $(HEADERS) $(LSCRIPTS) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUT_DIR)/%.o: %.asm $(HEADERS) $(LSCRIPTS) | $(OUT_DIR)
	$(AS) -o $@ $<

$(OUT_DIR):
	mkdir -p $@
