CC = gcc
CFLAGS= -std=c99 -Wall -O3
#CFLAGS= -std=c89 -Wall -Wextra -g

ifeq ($(OS),Windows_NT)
	CFLAGS += -IF:\Coding\SDL2-2.0.3\i686-w64-mingw32\include
	LDFLAGS = -LF:\Coding\SDL2-2.0.3\i686-w64-mingw32\lib -lmingw32 -lSDL2
else
	ifeq ($(shell uname -s),Linux)
		CFLAGS += 
		LDFLAGS = -lSDL2
	endif
	ifeq ($(shell uname -s),Darwin)
		CFLAGS += -F /Library/Frameworks
		LDFLAGS = -F /Library/Frameworks -framework SDL2
	endif
endif

#for linux:
#sudo apt-get install libsdl2-dev

#OBJS = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
# obj/%.o: src/%.c $(DEPS)
# 	$(CC) -c $(CFLAGS) -o $@ $<

OBJS = $(patsubst src/%.c, %.o, $(wildcard src/*.c))
DEPS = src/gameboy.h src/cpu.h

%.o: src/%.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

gb: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS)
