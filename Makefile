C := gcc
CFLAGS := -O2
LDFLAGS := $(shell pkg-config --libs jack) $(shell pkg-config --libs sdl2) -lm
COMPILE = ${C} ${DEPFLAGS} -std=c11 $(shell pkg-config --cflags sdl2) ${CFLAGS} $< -o $@ -c

SRCS := $(patsubst src/%,%,$(wildcard src/*.c))
OBJS := $(SRCS:%.c=%.o)

DEPFLAGS := -MD -MP

build/%.o: src/%.c
	@mkdir -p $(@D)
	${COMPILE}

build/rambajz: ${patsubst %,build/%,${OBJS}}
	${C} $^ -o $@ ${LDFLAGS}

all: build/rambajz

clean:
	rm -rf build

.PHONY: all build-dir clean
.DEFAULT_GOAL := all
-include Makefile.local
-include $(SRCS:%.c=build/%.d)
