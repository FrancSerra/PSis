all: chase_server chase_client client_bot

chase_server: chase_server.c chase.c chase.h
	gcc -Wall -pedantic chase_server.c chase.c -g -o chase_server -lncurses

chase_client: chase_client.c chase.c chase.h
	gcc -Wall -pedantic chase_client.c chase.c -g -o chase_client -lncurses

client_bot: client_bot.c chase.c chase.h
	gcc -Wall -pedantic client_bot.c chase.c -g -o client_bot -lncurses

clean:
	rm chase_server chase_client client_bot