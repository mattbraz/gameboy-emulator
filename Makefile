CC = gcc
CFLAGS = -std=c99 -Wall -O3
#CFLAGS= -std=c89 -Wall -Wextra -g

ifeq ($(OS), Windows_NT)
	EXE = gameboy.exe
	LIB = libgb.lib
	PYEXT = pygb.dll
	CFLAGS += -IF:\Coding\SDL2-2.0.3\i686-w64-mingw32\include
	LDFLAGS = -LF:\Coding\SDL2-2.0.3\i686-w64-mingw32\lib -lmingw32 -lSDL2
else
	EXE = gameboy
	LIB = libgb.a
	PYEXT = pygb.so
	ifeq ($(shell uname -s), Linux)
		CFLAGS += 
		LDFLAGS = -lSDL2
	endif
	ifeq ($(shell uname -s), Darwin)
		CFLAGS += -F /Library/Frameworks
		LDFLAGS = -F /Library/Frameworks -framework SDL2
	endif
endif

#for linux:
#sudo apt-get install libsdl2-dev

#OBJS = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
# obj/%.o: src/%.c $(DEPS)
# 	$(CC) -c $(CFLAGS) -o $@ $<

# SRCS = $(wildcard src/*.c)
# OBJS = $(patsubst src/%.c, %.o, $(wildcard src/*.c))
# DEPS = src/gameboy.h src/cpu.h

LIB_SRCS = $(wildcard src/core/*.c)
LIB_OBJS = $(patsubst src/core/%.c, build/%.o, $(LIB_SRCS))
LIB_DEPS = $(wildcard src/core/*.h)
#DEPS = core/src/gameboy.h core/src/cpu.h

build/main.o: src/main.c include/gameboy.h
	$(CC) -c $(CFLAGS) -I include -o $@ $<

build/sdl.o: src/frontend/sdl.c include/gameboy.h
	$(CC) -c $(CFLAGS) -I include -o $@ $<

build/%.o: src/core/%.c $(LIB_DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<
#	$(CC) -arch i386 -arch x86_64 -c $(CFLAGS) -o $@ $<

gameboy: build/main.o build/sdl.o $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

# $(SO): $(LIB_OBJS)
# 	$(CC) -shared -o $@ $^ $(LDFLAGS)

libgb.a: $(LIB_OBJS)
	ar rcs $@ $^
	
pygb: $(LIB) src/extension/pygb.c include/gameboy.h
	python setup.py build_ext -b . #-inplace
	#cp build/lib.macosx-10.12-intel-2.7/pygb.so .
	install_name_tool -add_rpath /Library/Frameworks/ pygb.so 
	#install_name_tool -change libgb.so /Users/matt/Coding/repos/gameboy-emulator/libgb.so pygb.so 


clean:
	rm -f $(OBJS) build/main.o $(EXE) $(PYEXT) $(LIB)
	rm -rf build
	mkdir build
