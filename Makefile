CC = g++
CFLAGS = -W -Wall -O3 -std=c++11 -g
LDFLAGS = -lSDL2main -lSDL2 -lstdc++
EXEC = vectorGraphics
SRCS = src

all: $(EXEC)

$(EXEC): vectorGraphics.o SDL2_gfxPrimitives.o
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LDFLAGS)

vectorGraphics.o: $(SRCS)/vectorGraphics.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

SDL2_gfxPrimitives.o: $(SRCS)/SDL2_gfxPrimitives.c $(SRCS)/SDL2_gfxPrimitives.h
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)


clean:
	rm *.o
	rm $(EXEC)
