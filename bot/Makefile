gyges: board.o display.o game_versus_bot.o bot.o
	gcc -Wall display.o board.o game_versus_bot.o bot.o -o gyges

game_versus_bot.o: game_versus_bot.c board.h display.h
	gcc -Wall -c game_versus_bot.c

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

game_debug.o: game_versus_bot.c board.h display.h
	gcc -Wall -D DEBUG -g -c game_versus_bot.c -o game_debug.o

clean:
	rm -f *.o gyges gyges_debug
