all:
	gcc main.c `sdl2-config --cflags --libs` -O -lSDL2_image -Wall -Werror -o a.out && ./a.out
