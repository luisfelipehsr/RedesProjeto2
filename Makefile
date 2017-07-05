##########################################################################
# MAKEFILE PARA O PROJETO 2 DE LABORATORIO DE REDES 1s2017							 #
# PROFESSOR: Luiz Fernando Bittencourt																	 #
# ALUNOS: 																															 #
# 	Joao Victor Baraldi Dourado - RA: 158044 														 #
# 	Luis Felipe Serrano 				- RA: 147091														 #
##########################################################################

##############################################
#  variaveis de compilacao e arquivos fonte  #
##############################################

CC = gcc
FLAGS = -pedantic -Wall
CLIENT = carClient
SERVERRAPIDO = closeServer
SERVERLENTO = lateServer
GERADOR = carGenerator.py
INICIADOR = startCars.py

##############################################
# variaveis a mudar de acordo com a execucao #
##############################################
NUM_CARROS = 5				# numero de carros que estarao presentes na simulacao
HOSTNAME = localhost	#	hostname do servidor principal

#compilacao dos arquivo em c
compile: $(CLIENT).c $(SERVERRAPIDO).c $(SERVERLENTO).c
	$(CC) $(CLIENT).c -o $(CLIENT) $(FLAGS)
	$(CC) $(SERVERLENTO).c -o $(SERVERLENTO) $(FLAGS)
	$(CC) $(SERVERRAPIDO).c -o $(SERVERRAPIDO) $(FLAGS)

#execucao do servidor de alta latencia
server1: $(SERVERRAPIDO)
	./$(SERVERRAPIDO)

#execucao do servidor de baixa latencia
server2: $(SERVERLENTO)
	./$(SERVERLENTO) $(HOSTNAME) $(NUM_CARROS)

#gera novos clientes aleatorio e esecuta os primeiros $(NUM_CARROS) deles
clients: $(CLIENT) $(GERADOR) $(INICIADOR)
	python3 $(GERADOR)
	python3 $(INICIADOR) $(NUM_CARROS) $(HOSTNAME)

#limpa os exeecutaveis C
clean:
	rm -f $(CLIENT) $(SERVERRAPIDO) $(SERVERLENTO)

#limpa os arquivos de saida dos clientes
clean_saidas:
	rm -f arq*.out

#executa o mesmo que os dois comandos acima
clean_all:
	rm -f arq*.out $(CLIENT) $(SERVERRAPIDO) $(SERVERLENTO) carParameters
