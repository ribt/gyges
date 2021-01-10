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

debug: board.o display_debug.o game_debug.o bot_debug.o
	gcc -Wall -g board.o display_debug.o game_debug.o bot_debug.o -o gyges_debug

display_debug.o: display.c board.h
	gcc -Wall -D DEBUG -g -c display.c -o display_debug.o

bot_debug.o: bot.c
	gcc -Wall -D DEBUG -g -c bot.c -o bot_debug.o

game_debug.o: game.c board.h display.h
	gcc -Wall -D DEBUG -g -c game.c -o game_debug.o

test: board.o display_debug.o bot_debug.o test.c
	gcc -Wall -g board.o display_debug.o bot_debug.o test.c -o test

bvb: board.o display_debug.o bot_debug.o bot-versus-bot.c
	gcc -Wall -g board.o display_debug.o bot_debug.o bot-versus-bot.c -o bvb

gui: board.o bot.o game_gui.o
	gcc -Wall board.o bot.o game_gui.o -lSDL2 -lSDL2_image -lSDL2_ttf -o gui

gui_debug: board.o bot.o game_gui_debug.o
	gcc -Wall board.o bot.o game_gui_debug.o -lSDL2 -lSDL2_image -lSDL2_ttf -o gui_debug

game_gui.o: game_gui.c
	gcc -Wall -c game_gui.c

game_gui_debug.o: game_gui.c
	gcc -Wall -g -D DEBUG -c game_gui.c -o game_gui_debug.o


clean:
	rm -f *.o gyges gyges_debug bvb test
