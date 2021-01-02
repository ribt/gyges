gyges: board.o display.o game.o bot.o
	gcc -Wall display.o board.o game.o bot.o -o gyges

game.o: game.c board.h display.h
	gcc -Wall -c game.c

board.o: board.c board.h
	gcc -Wall -c board.c

display.o: display.c board.h
	gcc -Wall -c display.c

bot.o: bot.c bot.h
	gcc -Wall -c bot.c

debug: board.o display_debug.o game_debug.o bot.o
	gcc -Wall -g board.o display_debug.o game_debug.o bot.o -o gyges_debug

display_debug.o: display.c board.h
	gcc -Wall -D DEBUG -g -c display.c -o display_debug.o

game_debug.o: game.c board.h display.h
	gcc -Wall -D DEBUG -g -c game.c -o game_debug.o

clean:
	rm -f *.o gyges gyges_debug
