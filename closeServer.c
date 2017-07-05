#include "header.h"
#include <errno.h>

#define LISTEN_PORT 12345
#define MAX_LINE 256
#define MAX_PENDING 5

void send_message(char *SENDTIME, char type, char modifier, char *data, int i,
				  int *clients) {
	char buf[MAX_LINE];
	confort conf;
	entertain ent;
	security secr;
	message msg;
	int src, clientfd;

	if (type == ENTERTAINMENT || type == CONFORT) {
		/* passa adiante hora de envio da mensagem */
		strcpy(msg.SENDTIME, SENDTIME);

		/* mensagem vinda de um carro */
		if (modifier == CARCLIENT && type == CONFORT) {
			msg.TYPE = CONFORT;
			memcpy(&conf, data, sizeof(conf));
			snprintf(conf.source, 20, "%d", i); // anota carro origem
			memcpy(&msg.data, &conf, sizeof(conf));
			clientfd = clients[0]; // envia para a nuvem
		}
		if (modifier == CARCLIENT && type == ENTERTAINMENT) {
			msg.TYPE = ENTERTAINMENT;
			memcpy(&ent, data, sizeof(ent));
			snprintf(ent.source, 20, "%d", i); // anota carro origem
			memcpy(&msg.data, &ent, sizeof(ent));
			clientfd = clients[0]; // envia para a nuvem
		}
		/* mensagem vinda do servico na nuvem */
		if (modifier == CLOUD && type == CONFORT) {
			/* Copia conteudo da mensagem */
			msg.TYPE = CONFORT;
			msg.MODIFIER = CLOUD;
			memcpy(&conf, data, sizeof(conf));
			memcpy(&msg.data, &conf, sizeof(conf));
			/* Reenvia para o carro de origem */
			src = atoi(conf.source);
			clientfd = clients[src];
		}

		if (modifier == CLOUD && type == ENTERTAINMENT) {
			/* Copia conteudo da mensagem */
			msg.TYPE = ENTERTAINMENT;
			msg.MODIFIER = CLOUD;
			memcpy(&ent, data, sizeof(ent));
			memcpy(&msg.data, &ent, sizeof(ent));
			/* Reenvia para o carro de origem */
			src = atoi(ent.source);
			clientfd = clients[src];
		}




	} else {
		msg.TYPE = type;
		msg.MODIFIER = modifier;
		memcpy(&msg.data, data, sizeof(secr));
		clientfd = clients[i];

	}

	memcpy(buf, &msg, MAX_LINE);
	if((clientfd > -1) && (send(clientfd, buf, MAX_LINE, 0) == -1)) {
		if (msg.MODIFIER == CLOUD)
			printf("ERROR: Couldn't send message to client %d\n", i);
		else
			printf("ERROR: Couldn't send message to cloud");
	}
}


/* Recebe um comando do cliente.
 * Retorna: -1 se um erro ocorreu, 0 se o cliente enviou um comando de exit ou
 *          1 se o cliente enviou uma mensagem. */
int recv_message(int clientfd, message *msg) {
	int res;
	char buffer[MAX_LINE];

	bzero(buffer, MAX_LINE); // limpa o buffer

	/* recebe e imprime o comando */
	res = recv(clientfd, buffer, MAX_LINE, 0);
	if (res == -1) {
		printf("ERROR: Couldn't receive from buffer\n");
		return -1;
	} else if (res == 0) {
		return 0;
	}

	memcpy(msg, buffer, MAX_LINE);

	return 1;
}

/* Fecha descritor de arquivo do cliente i */
void dropClient(int i, int *clients, fd_set *all_fds) {
	int clientfd;

	clientfd = clients[i];
	FD_CLR(clientfd, all_fds);
	close(clientfd);
	clients[i] = -1;
	if (i == 0) {
		printf("Cloud connection closed\n");
	} else {
		printf("Client %d connection closed\n", i);
	}
}

/* Recebe uma mensagem e a trata de acordo */
void processClient(int i, int *clients, fd_set *all_fds, car *cars) {
	char buf[MAX_LINE];
	char_car ccar;
	int clientfd, k, res, tempo_io, tempo_ij, tempo_fo, tempo_fj;
	message msg;
	size_t j;

	clientfd = clients[i];
	res = recv_message(clientfd, &msg);

	/* caso de erro ou conexao fechada pelo cliente */
	if (res == -1 || res == 0) {
		if (res == -1)
			printf("Dropping client %d\n", i);
		dropClient(i, clients, all_fds);
		return;
	}

	if (msg.TYPE == CONFORT || msg.TYPE == ENTERTAINMENT) {
		send_message(msg.SENDTIME, msg.TYPE, msg.MODIFIER,
					 msg.data, i, clients);
	} else {
		/* guarda informacoes do carro */
		memcpy(&ccar, &msg.data, sizeof(char_car));
		chartocar(&cars[i], &ccar);

		/* somente calcula colisoes com este carro se ele estiver em sen-
		 * tido do cruzamento */
	    if (((cars[i].dir == 1) && (cars[i].y*cars[i].sent > 0)) ||
			((cars[i].dir == 0) && (cars[i].x*cars[i].sent > 0))) {
			return;
		}

		/* checa por colisoes, se houver, envia os sinais */
		for (j = 1; j < FD_SETSIZE; j++) {
			if(clients[j] > 0 && cars[i].dir != cars[j].dir && i != j &&
			   cars[i].vel > 0 && cars[j].vel > 0 && clients[i] > 0){
				if (((cars[i].dir == 1)&&(cars[j].x*cars[j].sent < 0)) ||
					((cars[i].dir == 0)&&(cars[j].y*cars[j].sent < 0))) {
				    
					

					
					if (cars[i].dir == 0) {
						tempo_io = abs(cars[i].x / cars[i].vel);
						tempo_fo = abs(tempo_io + (cars[i].tam / cars[i].vel));
						tempo_ij = abs(cars[j].y / cars[j].vel);
						tempo_fj = abs(tempo_ij + (cars[j].tam / cars[j].vel));

						printf("i: %d %d, j: %d %d\n", cars[i].x, cars[i].vel, cars[j].y, cars[j].vel);

					} else {
						tempo_io = abs(cars[i].y / cars[i].vel);
						tempo_ij = abs(cars[j].x / cars[j].vel);
						tempo_fo = abs(tempo_io + (cars[i].tam / cars[i].vel));
						tempo_fj = abs(tempo_ij + (cars[j].tam / cars[j].vel));

						printf("i: %d %d, j: %d %d\n", cars[i].y, cars[i].vel, cars[j].x, cars[j].vel);
					}

					printf("tempos i: %d %d tempos j: %d %d\n", tempo_io, tempo_fo, tempo_ij, tempo_fj);
					
					if ((tempo_io - tempo_ij < ) ||
						(tempo_ij - tempo_io < ) {
						/* chamar ambulancia */
						for(k = 1; k < FD_SETSIZE; k++) {
							if (clients[k] > 0)
								send_message(msg.SENDTIME, msg.TYPE, BREAK,
											 msg.data, k, clients);
							if (i != k && j != k && clients[k] > 0)
								send_message(msg.SENDTIME, msg.TYPE,
											 CALL_RESCUE, msg.data, k,
											 clients);
						}

						/* elimina os carros que bateram */
						dropClient(i, clients, all_fds);
						dropClient(j, clients, all_fds);
						
					if ((tempo_ij >= tempo_io) &&
							   (tempo_ij <= tempo_fo)) {
						/* i acelera, j freia */
						send_message(msg.SENDTIME, msg.TYPE, ACCELERATE,
									 msg.data, i, clients);
						send_message(msg.SENDTIME, msg.TYPE, BREAK,
									 msg.data, j, clients);
						
					} else if ((tempo_io >= tempo_ij) &&
							   (tempo_io <= tempo_fj)) {
						/* i freia, j acelera */
				    	send_message(msg.SENDTIME, msg.TYPE, BREAK,
									 msg.data, i, clients);
						send_message(msg.SENDTIME, msg.TYPE, ACCELERATE,
									 msg.data, j, clients);
						/* i freia, j acelera */
					} // fim if havera colisao		
				} // fim if colisao
			} // fim if direcoes ortogonais
		} // fim if carro indo ao cruzamento
	} // fim for comparacao com demais carros

}

/* Funcao principal. Ignora argumentos */
int main() {

	struct sockaddr_in socket_address, client_address, conf_address;
	char client_ip[INET_ADDRSTRLEN];
	int listenfd, clientfd, res, n_ready;
	int maximumfd, maximumfd_index, clients[FD_SETSIZE], i;
    fd_set read_set, all_fds;
	unsigned int addrlen;
	unsigned short client_port;
	car cars[FD_SETSIZE];

	/* criação da estrutura de dados de endereço */
	bzero((char *)&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(LISTEN_PORT);
    socket_address.sin_addr.s_addr = INADDR_ANY;

    /* criação de socket passivo */
	listenfd = socket(AF_INET,SOCK_STREAM, 0);
	if (listenfd == -1) {
		printf("ERROR: Couldn't create socket\n");
		return 0;
	}

	/* Associa socket ao descritor */
	res = bind(listenfd, (struct sockaddr *) &socket_address,
			   sizeof(socket_address));
	if (res == -1) {
		printf("ERROR: Couldn't bind server\n");
		return 0;
	}

	/* Cria escuta do socket para aceitar conexões */
    listen(listenfd,MAX_PENDING);

	/* Inicializa a lista de clientes e guarda o indice/dados do ultimo */
	maximumfd = listenfd;
	maximumfd_index = -1;
	for (i = 0; i < FD_SETSIZE; i++) // maximo 1024 conexoes
		clients[i] = -1;

	/* Inicia conjunto de bits representando os descritores */
	FD_ZERO(&all_fds);
	FD_SET(listenfd, &all_fds);

	/* Aguarda e aceita ate 1024 conexoes simultaneas */
	while (1) {

		/* usa read_set para saber os clientes com dados para leitura */
		read_set = all_fds;
		n_ready = select(maximumfd+1, &read_set, NULL,NULL,NULL);
		if (n_ready < 0) {
			printf("ERROR Couldn't use select\n");
			if (errno == EBADF)
				printf("ERROR: Select on invalid filedescriptor");
			exit(1);
		}

		/* verifica nova conexao de acordo com FD_ISSET sobre o listen */
		if(FD_ISSET(listenfd, &read_set)) {
			addrlen = sizeof(client_address);
			clientfd = accept(listenfd,(struct sockaddr *) &client_address,
							  &addrlen);
			if (clientfd == -1) {
				printf("ERROR: Couldn't accept connection\n");
				exit(1);
			}

			/* Procura um slot na lista de clientes vazio */
			res = 0;
			i = -1;
			while (!res) {
				i++;
				if(clients[i] < 0) {
					clients[i] = clientfd;
					res = 1;
				}
			}

			/* Termina se houve estouro do numero maximo de clientes */
			if (i == FD_SETSIZE) {
				printf("ERROR Numero maximo de clientes atingido.\n");
				exit(1);
			}

			/* Imprime informacoes das portas e IPs do cliente */
			if (i == 0)
				printf("Servico na nuvem conectado.\n");
			else
				printf("Novo carro conectado. Indice local: %d\n", i);
			addrlen = sizeof(conf_address);
			res = getpeername(clientfd, (struct sockaddr *) &conf_address,
							  &addrlen);
			if (res == -1) {
				printf("ERROR: Couldn't get client %d address\n", i);
			    clients[i] = -1;
				close(clientfd);
				continue;
			}
			inet_ntop(AF_INET, &(conf_address.sin_addr), client_ip,
					  INET_ADDRSTRLEN);
			client_port = htons(conf_address.sin_port);
			printf("Um cliente se conectou: %s:%u\n", client_ip,
				   client_port);

			/* Adiciona novo cliente ao conjunto */
			FD_SET(clientfd, &all_fds);

			/* Incrementa o valor maximo de descritor se preciso */
			if (clientfd > maximumfd)
				maximumfd = clientfd;
			if (i > maximumfd_index)
				maximumfd_index = i;

			if (--n_ready <= 0)
				continue;
		} // fim if FD_ISSET listen

		/* se um cliente existe num dado indice e enviou algum conteudo, recebe
		 * mensagem e processa de acordo. Envia respostas ou, se houver um erro,
		 * remove o cliente */
		for (i = 0; i <= maximumfd_index; i++) {
			clientfd = clients[i];
			if (clientfd >= 0 && FD_ISSET(clientfd, &read_set))
				processClient(i, clients, &all_fds, cars);
		} // fim for FD_ISSET clientfd
	} // fim while(1)

	/* Fecha descritor do socket de listen e de todos os accept*/
	printf(" Server disabled\n");
	for (i = 0; i < FD_SETSIZE; i++) {
		clientfd = clients[i];
		if (clientfd >= 0)
			close(clientfd);
	}
	close(listenfd);

	return 0;
}
