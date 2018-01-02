C := gcc
CFLAGS := -std=c11 -O2
LDFLAGS := $(shell pkg-config --libs jack)
objs := jack.o rambajz.o

build-dir:
	mkdir -p build

build/%.o: src/%.c build-dir
	${C} ${CFLAGS} $< -o $@ -c

build/rambajz: ${patsubst %,build/%,${objs}}
	gcc $^ -o $@ ${LDFLAGS}

all: build/rambajz

clean:
	rm -rf build

.PHONY: all build-dir clean
.DEFAULT_GOAL := all
