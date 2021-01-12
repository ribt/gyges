gyges: board.o game.o bot.o
	gcc -Wall board.o game.o bot.o -lSDL2 -lSDL2_image -lSDL2_ttf -o gyges

cli: board.o display.o game_cli.o bot.o
	gcc -Wall display.o board.o game_cli.o bot.o -o gyges_cli

game.o: game.c board.h bot.h
	gcc -Wall -c game.c

board.o: board.c board.h
	gcc -Wall -c board.c

display.o: display.c board.h
	gcc -Wall -c display.c

bot.o: bot.c bot.h
	gcc -Wall -c bot.c

game_cli.o: game_cli.c board.h display.h
	gcc -Wall -c game_cli.c

clean:
	rm -f *.o gyges gyges_cli