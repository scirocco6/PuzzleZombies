all:	PuzzleZombies

clean:
	rm -f PuzzleZombies *.o

word.o:	word.c
	gcc -g -c word.c `sdl-config --cflags`

collision.o:	collision.c
	gcc -g -c collision.c `sdl-config --cflags`

zombie.o:	zombie.c
	gcc -g -c zombie.c `sdl-config --cflags`

audio.o:	audio.c
	gcc -g -c audio.c `sdl-config --cflags`

player.o:	player.c
	gcc -g -c player.c `sdl-config --cflags`

screen.o:	screen.c
	gcc -g -c screen.c `sdl-config --cflags`

input.o:	input.c 
	gcc -g -c input.c `sdl-config --cflags`

PuzzleZombies:	main.c word.o collision.o zombie.o audio.o player.o screen.o input.o
	gcc -g main.c word.o collision.o zombie.o audio.o player.o screen.o input.o -o PuzzleZombies `sdl-config --cflags --libs` -lSDL_ttf
