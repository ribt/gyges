gyges: display.o board.o game.c
	gcc -Wall display.o board.o game.c -o gyges

display.o: board.h display.c
	gcc -Wall -c display.c

debug: display.o board.o game.c
	gcc -Wall -D DEBUG -g display.o board.o game.c -o gyges_debug

clean:
	rm display.o
