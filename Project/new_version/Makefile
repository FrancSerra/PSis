all: chase_server chase_client

chase_server: chase_server.c lists.c chase.c chase.h lists.h
	gcc -Wall -pedantic chase_server.c lists.c chase.c -g -o chase_server -lncurses

chase_client: chase_client.c chase.h
	gcc -Wall -pedantic chase_client.c -g -o chase_client -lncurses

clean:
	rm chase_server chase_client