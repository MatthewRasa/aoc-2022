CC := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -pthread
BUILD := release

ifeq ($(BUILD),debug)
CXXFLAGS += -g -O0
else ifeq ($(BUILD),release)
CXXFLAGS += -O3
endif

CLIST := $(shell find src -name "*.cc")
OLIST := $(CLIST:src/%.cc=obj/%.o)
BLIST := $(OLIST:obj/%.o=bin/%)

.PHONY: all clean
.SECONDARY: $(OLIST)

all: $(BLIST)

bin/%: obj/%.o
	@mkdir -p $(shell dirname $@)
	$(CC) -o $@ $^

obj/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) -o $@ -c $<

clean:
	@rm -vrf bin/ obj/
