NAME     := $(notdir $(CURDIR))
CC       := gcc
CXX      := g++
STRIP    := strip
OBJCOPY  := objcopy
CFLAGS   := -Wall -Wextra -Werror -Wpedantic
CXXFLAGS := -Wall -Wextra -Werror -Wpedantic -fno-exceptions -fno-rtti
LDFLAGS  := -s -T linker.ld

OUT_DIR  := $(BUILD_DIR)/$(NAME)
TARGET   := $(BUILD_DIR)/$(NAME).bin

C_SRCS   := $(wildcard *.c)
CXX_SRCS := $(wildcard *.cpp)
HEADERS  := $(wildcard *.h *.hpp)
OBJS     := $(patsubst %.c,$(OUT_DIR)/%.o,$(C_SRCS)) \
            $(patsubst %.cpp,$(OUT_DIR)ca/%.o,$(CXX_SRCS))

LINKER   := $(if $(CXX_SRCS),$(CXX),$(CC))

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -o $(patsubst %.bin,%.elf,$@) $^
	$(OBJCOPY) -O binary $(patsubst %.bin,%.elf,$@) $@
	rm $(patsubst %.bin,%.elf,$@)

$(OUT_DIR)/%.o: %.c $(HEADERS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/%.o: %.cpp $(HEADERS) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUT_DIR):
	mkdir -p $@
