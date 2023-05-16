CC = g++
CFLAGS = -W -Wall -O3 -std=c++11 -g
LDFLAGS = -lSDL2main -lSDL2 -lstdc++
EXEC = vectorGraphics
SRCS = src

all: $(EXEC)

$(EXEC): vectorGraphics.o SDL2_gfxPrimitives.o point.o mesh.o world.o
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LDFLAGS)

vectorGraphics.o: $(SRCS)/vectorGraphics.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

point.o: $(SRCS)/point.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

mesh.o: $(SRCS)/mesh.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

world.o: $(SRCS)/world.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

SDL2_gfxPrimitives.o: $(SRCS)/SDL2_gfxPrimitives.c $(SRCS)/SDL2_gfxPrimitives.h
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)


clean:
	rm *.o
	rm $(EXEC)
