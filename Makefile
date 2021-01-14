gyges: board.o game.o bot.o
	gcc -Wall board.o game.o bot.o -lSDL2 -lSDL2_image -lSDL2_ttf -o gyges

gyges_cli: board.o game_cli.o bot.o
	gcc -Wall board.o game_cli.o bot.o -o gyges_cli

game.o: game.c board.h bot.h
	gcc -Wall -c game.c

board.o: board.c board.h
	gcc -Wall -c board.c

bot.o: bot.c bot.h
	gcc -Wall -c bot.c

game_cli.o: game_cli.c board.h
	gcc -Wall -c game_cli.c

cli: gyges_cli

clean:
	rm -f *.o gyges gyges_cli