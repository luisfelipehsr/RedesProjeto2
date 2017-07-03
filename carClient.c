#include "header.h"

#define SERVER_PORT 12345
#define MAX_LINE 256
#define COMMAND_MSG 1
#define COMMAND_EXIT 0

int send_message(int mode, int socketfd, car myself, int app, int url,
				 time_t time, time_register *tr) {
	message msg;
	security secr;
	confort conf;
	entertain ent;
	char buf[MAX_LINE];
	int res;

	if (mode == SECURITY) {
		
		msg.TYPE = SECURITY;
		msg.MODIFIER = CAR_REPORT;
		secr.carInfo = myself;
		memcpy(&msg.data, &secr, sizeof(secr));
		memcpy(buf, &msg, sizeof(msg));
		tr->last_security = time;

		printf("id: %d %d x: %d %d\n", myself.id, secr.carInfo.id, myself.x, secr.carInfo.x);
	
	} else if (mode == CONFORT) {
		msg.TYPE = CONFORT;
		if (url == 0) {
			strcpy(conf.url, URL_FACEBOOK);
			strcpy(conf.text, "Olha essa foto minha dirigindo");
		}

		if (url == 1) {
			strcpy(conf.url, URL_TWITTER);
			strcpy(conf.text, "dirijo bem #tweetdirigindo");
		}

		memcpy(&msg.data, &conf, sizeof(conf));
		memcpy(buf, &msg, sizeof(msg));

		printf("%s\n", buf+66);

	}

	return send(socketfd, &buf, MAX_LINE, 0);
}


/* Recebe um comando do usuario e envia para o servidor.
 * Retorna: -1 se houve um erro, 0 se foi um comando de exit ou
 *          1 se foi uma mensagem. */
int user_command (int socketfd, char *buffer, unsigned long buffer_size) {
	int res;

	/* limpa buffer e recebe linha */
	bzero(buffer, buffer_size);
	fgets(buffer, buffer_size, stdin);

	/* caso seja um comando de saida */
	if (strcmp(buffer,"exit\n") == COMMAND_EXIT) {
		res = send(socketfd, buffer, buffer_size - 1, 0);
		if (res == -1) {
			printf("ERROR: Couldn't send exit command to server\n");
			return -1;
		} else {
			return COMMAND_EXIT;
		}
	}

	/* envia mensagem ao servidor */
  res = send(socketfd, buffer, buffer_size - 1, 0);
	if (res == -1) {
		printf("ERROR: Couldn't send message to server\n");
		return -1;
	}

	return COMMAND_MSG;
}

/* Recebe uma resposta do servidor e a imprime.
 * Retorna: -1 caso haja um erro ou 1 caso seja sucedido */
int server_response (int socketfd, char *buffer, unsigned long buffer_size) {
	int res;

	bzero(buffer, buffer_size); // limpa buffer

	/* recebe mensagem */
	res = recv(socketfd, buffer, buffer_size - 1, 0);
	if (res == -1) {
		printf("ERROR: Couldn't receive response from server\n");
		return -1;
	}

	printf("Received from server: %s\n", buffer);

	return COMMAND_MSG;
}

void buildCar(car *myself, char *argv[], int *app, int *url, int *reckless) {
	myself->id = atoi(argv[2]);
	myself->x = atoi(argv[3]);
	myself->y = atoi(argv[4]);
	myself->vel = atoi(argv[5]);
	myself->dir = atoi(argv[6]);
	myself->sent = atoi(argv[7]);
	myself->tam = atoi(argv[8]);
	*app = atoi(argv[9]);
	*url = atoi(argv[10]);
	*reckless = atoi(argv[11]);
}

/* Funcao principal. Precisa de 2 e somente 2 argumentos, o nome do servidor
   e o numero do arquivo de configuracao de carro a ser lido */
int main (int argc, char* argv[]) {

	struct hostent *host_address;
	struct sockaddr_in socket_address, conf_address;
	struct timeval tv;
	const char *host;
	car myself;
	message msg;
	char buf[MAX_LINE], client_ip[INET_ADDRSTRLEN];
	int carNumber, hasCommands, command, response, socketfd, res, waiting;
	int callingHelp;
	unsigned addrlen;
	unsigned short client_port;
	size_t len;
	time_register tr;
	time_t time, START_MOVING;
	int app, url, reckless;

	/* verificação de argumentos */
	if (argc == 13) {
		host = argv[1];
		carNumber = atoi(argv[2]);
		buildCar(&myself, argv, &app, &url, &reckless);
		printf("Car %d created\n", carNumber);

	} else {
		printf("ERROR: Twelve arguments should be provided. %d given.\n", argc-1);
		return 0;
	}

	/* tradução de nome para endereço IP */
	host_address = NULL;
	host_address = gethostbyname(host);

	if (host_address == NULL) {
		printf("ERROR: Host name not found\n");
		return 0;
	}

	/* criação da estrutura de dados de endereço para TCP IPv4*/
	bzero((char *)&socket_address, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(SERVER_PORT);
	len = host_address->h_length;
	bcopy((char *)host_address->h_addr_list[0],
		  (char *)&socket_address.sin_addr.s_addr, len);

	/* criação de socket ativo*/
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	if (socketfd == -1) {
		printf("ERROR: Couldn't create socket\n");
		return 0;
	}

	/* estabelecimento da conexão */
	res = connect(socketfd,(const struct sockaddr *)&socket_address,
				  sizeof(socket_address));
	if (res == -1) {
		printf("ERROR: Couldn't connect to server address\n");
		return 0;
	}

	/* Imprime informacoes das portas e IPs do cliente */
	addrlen = sizeof(conf_address);
	res = getsockname(socketfd, (struct sockaddr *) &conf_address, &addrlen);
	if (res == -1) {
		printf("erro ao obter endereço do cliente\n");
		return 0;
	}
	inet_ntop(AF_INET, &(conf_address.sin_addr), client_ip, INET_ADDRSTRLEN);
	client_port = htons(conf_address.sin_port);
	printf("Conectando-se com endereco ip e porta: %s:%u\n", client_ip,
		   client_port);

	/*timeout de 2ms para o recv */
	tv.tv_sec = 0;
	tv.tv_usec = 2000; // == 2 ms
	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,
			   sizeof(struct timeval));


	/* Inicializa o registrador de horarios. Ele eh utilizado para saber inter-
       valos de tempo para as acoes */
	time = get_time();
	buildTimeRegister(&tr, time);


	/* Loop que faz cagadas no transito */
	/*
ta na hora(tipo intervalo, tempo x, tempo atual):
 			   tempo atual - tempo x
			   se eh atualizacao ou seguranca:
			   	  tempo atual- tempo x >= 1
			   se eh entretenimento:
			   	  tempo atual - tempo x >= 0.03
			   se eh conforto:
            tempo atual - tempo x >= 0.05

				essas coisas retornam true ou false



			enquanto() :
				checar a hora

				se nao esta esperando E nao eh reckless E ta na hora:
				   atualiza velocidade (pode receber um incremento rand)
				   atualiza posicao
				   atualiza tempo da ultima atualizacao
				se eh pra chamar a ambulancia:
				   print("VOU BOTAR NO YOUTUBE")

				se ta na hora de seguranca:
				   envia mensagem de seguranca

				se ta na hora de entretenimento:
				   gera mensagem aleatoria de entretenimento
				   manda mensagem
				se conforto:
				   mesmo de cima

				recebe uma mensagem (timeout de 0.01, vulgo 10ms good ping)
				se recebeu mensagem:
				    se eh de seguranca:
				        se eh de frear:
					  	    velocidade <- 0
						se eh de acelerar:
						    velocidade + 5
					  se eh de chamar ambulancia:
						    chamar ambulancia <- True
					se eh de entretenimento:
					   print("Jogo para quem esta dirigindo recebeu msg")
					se eh de lazer/conforto:
					   print("Pessoa que voce nao vai pegar postou foto")

          se deu timeout:
				     faca nada
			  volta loop

	 */


	//=====================================================================
	//LUIS, NAO QUER CRIAR UMA THREAD QUE FICA ESPERANDO AS RESPOSTAS? AI, NAO POE TIMEOUT
	//=====================================================================
	waiting = 0; // nao esta esperando
	callingHelp = 0; // nao esta chamando ajuda
	while (!isTime(STOP_SIMULATION, time, &tr)) {
		time = get_time();

		if (isTime(UPDATE, time, &tr)) {
			//==================================
			//========== NOT WAITING? ==========
			//==================================
			if (callingHelp)
				printf("Vou botar no youtube!!!!!!\n");
			
			if (!waiting || reckless){
				srand((int) time);
				myself.vel += (rand() % 10) + 10;
			}

			if (myself.dir == 0)
				myself.x += (myself.vel)*(myself.sent);

			else if(myself.dir == 1)
				myself.y += (myself.vel)*(myself.sent);
			/*
			msg.TYPE = SECURITY;
			msg.MODIFIER = CAR_REPORT;
			memcpy(&msg.data, &myself, sizeof(car));
			memcpy(&buf, &msg, sizeof(message));

			send(socketfd, &buf, MAX_LINE, 0);
			*/
			if (send_message(SECURITY, socketfd, myself, app, url, time,
							 &tr) == -1) {
				printf("ERROR: Couldn't send security message to server\n");
				return 0;
			}
			
			printf("ta funfando\n");
		}
	}

	/* fecha descritor */
	close(socketfd);

	return 0;
}
