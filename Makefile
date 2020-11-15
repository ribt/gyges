game: display.o board.o game.o
	gcc -Wall display.o board.o game.o -o game

display.o: board.h display.c
	gcc -Wall -c display.c

game.o: board.h display.h game.c
	gcc -Wall -c game.c

clean:
	rm display.o game.o
