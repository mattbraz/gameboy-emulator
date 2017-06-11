CC = gcc
CFLAGS = -std=c99 -Wall -O3

ifeq ($(OS), Windows_NT)
	EXE = gameboy.exe
	LIB = libgb.a
	LDFLAGS = -lmingw32 -lSDL2
else
	EXE = gameboy
	LIB = libgb.a
	ifeq ($(shell uname -s), Linux)
		LDFLAGS = -lSDL2
	endif
	ifeq ($(shell uname -s), Darwin)
		CFLAGS += -F /Library/Frameworks
		LDFLAGS = -F /Library/Frameworks -framework SDL2
	endif
endif

LIB_SRCS = $(wildcard src/core/*.c)
LIB_OBJS = $(patsubst src/core/%.c, build/%.o, $(LIB_SRCS))
LIB_DEPS = $(wildcard src/core/*.h)

build/main.o: src/main.c include/gameboy.h
	$(CC) -c $(CFLAGS) -I include -o $@ $<

build/sdl.o: src/frontend/sdl.c include/gameboy.h
	$(CC) -c $(CFLAGS) -I include -o $@ $<

build/%.o: src/core/%.c $(LIB_DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<
#	$(CC) -arch i386 -arch x86_64 -c $(CFLAGS) -o $@ $<

gameboy: build/main.o build/sdl.o build/libgb.a  #$(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

# $(SO): $(LIB_OBJS)
# 	$(CC) -shared -o $@ $^ $(LDFLAGS)

build/libgb.a: $(LIB_OBJS)
	ar rcs $@ $^
	
clean:
	rm -f $(LIB_OBJS) build/main.o $(EXE) $(LIB)
	rm -rf build
	mkdir build
