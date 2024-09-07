CC = gcc
CFLAGS = -Wall
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: game

game: main.c
	$(CC) $(CFLAGS) -o game.out main.c $(LIBS)

clean:
	rm -f game