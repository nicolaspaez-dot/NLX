CC=gcc
CFLAGS=-Iinclude -Wall -Wextra -std=c99
LIBS=-lncurses -lpcap -lsqlite3 -lcurl -lpthread
SRC=src/main.c src/ui.c src/collector.c src/analyzer.c src/storage.c src/renderer.c src/utils.c
OUT=build/nx

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

debug:
	mkdir -p build
	$(CC) $(CFLAGS) -g -DDEBUG $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -rf build

install: all
	@echo "Instalando NLX..."
	@sudo ./install.sh

uninstall:
	@echo "Desinstalando NLX..."
	@sudo ./uninstall.sh

test: debug
	@echo "Ejecutando pruebas..."
	@./build/nx --test

.PHONY: all debug clean install uninstall test 