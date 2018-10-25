CC = g++
CFLAGS = -W -Wall -O3 -std=c++11 -g -fopenmp
LDFLAGS = -lSDL2main -lSDL2 -lstdc++
EXEC = vectorGraphics

all: $(EXEC)

$(EXEC): vectorGraphics.o SDL2_gfxPrimitives.o
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LDFLAGS)

vectorGraphics.o: vectorGraphics.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

SDL2_gfxPrimitives.o: SDL2_gfxPrimitives.c SDL2_gfxPrimitives.h
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)


clean:
	rm *.o
	rm $(EXEC)
