BUILD_DIR := $(CURDIR)/build
SUBDIRS   := $(patsubst %/,%,$(sort $(dir $(wildcard */))))
SUBDIRS   := $(filter-out build,$(SUBDIRS))

export BUILD_DIR

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -rf $(BUILD_DIR)
