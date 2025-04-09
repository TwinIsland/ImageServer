CC := gcc
CFLAGS := -fPIC -Wall -Wextra -I../includes
LDFLAGS := -shared

MODULE_DIR := ./
MODULES := $(shell find $(MODULE_DIR) -type f -name '*.c' -not -path '*/examples/*')
TARGETS := $(addsuffix .ismod,$(basename $(MODULES)))

.PHONY: all clean

all: $(TARGETS)

%.ismod: $(MODULE_DIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	mv $@ $(shell pwd)

clean:
	rm -f *.ismod