

CC = gcc
FLAGS = -pedantic -Wall
TRABALHO = p2
CLIENT = carClient
SERVERRAPIDO = closeServer
SERVERLENTO = lateServer
GERADOR = carGenerator.py
INICIADOR = startCars.py

NUM_CARROS = 5
HOSTNAME = localhost

compile: $(CLIENT).c $(SERVERRAPIDO).c $(SERVERLENTO).c
	$(CC) $(CLIENT).c -o $(CLIENT) $(FLAGS)
	$(CC) $(SERVERLENTO).c -o $(SERVERLENTO) $(FLAGS)
	$(CC) $(SERVERRAPIDO).c -o $(SERVERRAPIDO) $(FLAGS)

server1: $(SERVERRAPIDO)
	./$(SERVERRAPIDO)

server2: $(SERVERLENTO)
	./$(SERVERLENTO) $(HOSTNAME) $(NUM_CARROS)

clients: $(CLIENT) $(GERADOR) $(INICIADOR)
	python3 $(GERADOR)
	python3 $(INICIADOR) $(NUM_CARROS) $(HOSTNAME)

clean:
	rm -f $(CLIENT) $(SERVERRAPIDO) $(SERVERLENTO)

clean_saidas:
	rm -f arq*.out
