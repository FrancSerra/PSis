all: chase_server chase_client

chase_server: ./server/chase_server.c ./server/fcn_server.c ./server/chase.c ./server/fcn_server.h ./server/chase.h
	gcc -Wall -pedantic ./server/chase_server.c ./server/fcn_server.c ./server/chase.c -g -o chase_server -lncurses -lpthread

chase_client: ./client/chase_client.c ./client/fcn_client.c ./client/chase.c ./client/fcn_client.h ./client/chase.h
	gcc -Wall -pedantic ./client/chase_client.c ./client/fcn_client.c ./client/chase.c -g -o chase_client -lncurses -lpthread

clean:
	rm chase_server chase_client