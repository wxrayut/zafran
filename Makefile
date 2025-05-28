
UNAME_S := $(shell uname -s)

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c99 -O2 -Wno-implicit-function-declaration -Iinclude

# Source files
SRC := src/zafran.c
HDR := include/zafran.h
OUT := build
DEMO := demo.c

# Install paths
# PREFIX ?= /usr/local
# INCLUDEDIR := $(PREFIX)/include
# LIBDIR := $(PREFIX)/lib


all: demo


demo: $(SRC) $(DEMO)
	$(CC) $(CFLAGS) -o demo $(SRC) $(DEMO)


lib:
ifeq ($(OS),Windows_NT)
	$(CC) -DZAFRAN_EXPORTS -shared -Iinclude -o $(OUT)/zafran.dll $(SRC)
else ifeq ($(UNAME_S),Linux)
	$(CC) -fPIC -shared -Iinclude -o $(OUT)/libzafran.so $(SRC)
else ifeq ($(UNAME_S),Darwin)
	$(CC) -fPIC -shared -Iinclude -o $(OUT)/libzafran.dylib $(SRC)
endif


clean:
	rm -f demo
	rm -f $(OUT)/zafran.dll $(OUT)/libzafran.so $(OUT)/libzafran.dylib
	rm -rf $(OUT)


$(shell mkdir -p $(OUT))


.PHONY: all demo lib install uninstall clean

