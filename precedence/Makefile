OPTIMIZATIONS=-O3
DEBUG=-ggdb3
CC=clang

CFLAGS=-Wall -Wextra -std=c17 -pedantic -ggdb -Wno-gnu-empty-initializer

build: clean
	mkdir -p build/
	$(CC) $(CFLAGS) $(DEBUG) -o build/precedence src/precedence.c

format:
	clang-format -i src/*.c src/*.h

clean:
	rm -f build/*

.DEFAULT_GOAL := build
.PHONY: build
