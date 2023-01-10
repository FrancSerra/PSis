all: chase_server chase_client

chase_server: chase_server.c chase.c chase.h
	gcc -Wall -pedantic chase_server.c chase.c -g -o chase_server -lncurses -lpthread

chase_client: chase_client.c chase.c chase.h
	gcc -Wall -pedantic chase_client.c chase.c -g -o chase_client -lncurses -lpthread

clean:
	rm chase_server chase_client