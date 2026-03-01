NAME     := $(notdir $(CURDIR))
CC       := gcc
CXX      := g++
CFLAGS   := -Wall -Wextra -Werror -Wpedantic
CXXFLAGS := -Wall -Wextra -Werror -Wpedantic

OUT_DIR  := $(BUILD_DIR)/$(NAME)
TARGET   := $(BUILD_DIR)/$(NAME).elf

C_SRCS   := $(wildcard *.c)
CXX_SRCS := $(wildcard *.cpp)
HEADERS  := $(wildcard *.h *.hpp)
OBJS     := $(patsubst %.c,$(OUT_DIR)/%.o,$(C_SRCS)) \
            $(patsubst %.cpp,$(OUT_DIR)/%.o,$(CXX_SRCS))

LINKER   := $(if $(CXX_SRCS),$(CXX),$(CC))

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -o $@ $^

$(OUT_DIR)/%.o: %.c $(HEADERS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/%.o: %.cpp $(HEADERS) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUT_DIR):
	mkdir -p $@
