C := gcc
CFLAGS := -O2
COMPILE = ${C} -std=c11 $(shell pkg-config --cflags sdl2) ${CFLAGS} $< -o $@ -c
LDFLAGS := $(shell pkg-config --libs jack) $(shell pkg-config --libs sdl2) -lm
objs := jack.o rambajz.o

build-dir:
	mkdir -p build

build/%.o: src/%.c build-dir
	${COMPILE}

build/rambajz: ${patsubst %,build/%,${objs}}
	${C} $^ -o $@ ${LDFLAGS}

all: build/rambajz

clean:
	rm -rf build

.PHONY: all build-dir clean
.DEFAULT_GOAL := all
