/***************************************
****** Servidor de alta latencia *******
****************************************/
#include "header.h"

#define MAX_MSG_CAR 20
#define SERVER_PORT 12345
#define MAX_LINE 256

void remove_msg(message_buffer *msgbuf, int i, int n_msgs) {
  int j;
  for (j = i; j < n_msgs; j++) {
    msgbuf[j].timestamp = msgbuf[j+1].timestamp;
    msgbuf[j].msg.TYPE = msgbuf[j+1].msg.TYPE;
    msgbuf[j].msg.MODIFIER = msgbuf[j+1].msg.MODIFIER;
    strcpy(msgbuf[j].msg.data, msgbuf[j+1].msg.data);
  }
}

int answer(int sockfd, message msg, msg_counter *mc){
	char buf[MAX_LINE];
	message out;
	entertain ent, entCopy;
	confort conf, confCopy;
	int n;

	strcpy(out.SENDTIME, msg.SENDTIME);

	srand((int) get_time());
	n=rand()%5;

	/* Modifica mensagem de saida conforme seu tipo e conteudo */
	if (msg.TYPE == CONFORT) {
		mc->sent_confort++;
		memcpy(&conf, &msg.data, sizeof(confort));
		out.TYPE = msg.TYPE;
		out.MODIFIER = CLOUD;

		// copia para confCopy url e source
		strcpy(confCopy.url, conf.url);
	  strcpy(confCopy.source, conf.source);

		// adiciona em confCopy outros dados
		if (strcmp(conf.url, URL_FACEBOOK) == 0) {
			if (n < 3) {
				snprintf(confCopy.text, sizeof(confCopy.text),
						 "A pessoa que voce NAO quer pegar curtiu seu status: %s",
						 conf.text);

			} else {
				snprintf(confCopy.text, sizeof(confCopy.text),
						 "A pessoa que voce QUER pegar curtiu seu status: %s",
						 conf.text);
			}
		} else if (strcmp(conf.url, URL_TWITTER) == 0) {
			if (n < 3) {
				snprintf(confCopy.text, sizeof(confCopy.text),
						 "A pessoa que voce NAO quer pegar retweetou: %s",
						 conf.text);
			} else {
				snprintf(confCopy.text, sizeof(confCopy.text),
						 "A pessoa que voce QUER pegar retweetou: %s",
						 conf.text);
			}
		} // fim if URLs

		/* adiciona ao buffer a mensagem de saida */
		memcpy(&out.data, &confCopy, sizeof(confort));
		memcpy(buf, &out, sizeof(message));

	} else if (msg.TYPE == ENTERTAINMENT) {
		mc->sent_entertainment++;
		memcpy(&ent, &msg.data, sizeof(entertain));
		out.TYPE = msg.TYPE;
		out.MODIFIER = CLOUD;

		// copia para entCopy nome do app e source
		strcpy(entCopy.appName, ent.appName);
		strcpy(entCopy.source, ent.source);

		// adiciona em entCopy outros dados
		if (strcmp(ent.appName, APP_TIBIA) == 0) {
			if (n < 3) {
				snprintf(entCopy.data, sizeof(entCopy.data),
						 "Outro jogador te matou e roubou suas coisas quando voce matou o orc\n");

			} else {
				snprintf(entCopy.data, sizeof(entCopy.data),
						 "Voce conseguiu derrotar o orc com sucesso! Congratz! GG!\n");
			}
		} else if (strcmp(ent.appName, APP_POKEMON) == 0) {
			if (n < 3) {
				snprintf(entCopy.data, sizeof(entCopy.data),
						 "The wild code fled...\n");
			} else {
				snprintf(entCopy.data, sizeof(entCopy.data),
						 "You caught the wild code! You won %d code candies\n",
						 n);
			}
		} // fim if AppName

		/* adiciona ao buffer a mensagem de saida */
		memcpy(&out.data, &entCopy, sizeof(entertain));
		memcpy(buf, &out, sizeof(message));

	} // fim if TYPE

	/* envia mensagem e retorna saida */
	return send(sockfd, &buf, MAX_LINE, 0);
}

void printUpdate(int numUpdate, msg_counter mc) {
	printf("\n\n------------------------\nUPDATE #%d:\n", numUpdate);
	printf("Total messages sent / received:\n");
	printf("SECURITY CONFORT ENTERTAINMENT\n");
	printf("%8d %7d %13d\n", mc.sent_security, mc.sent_confort,
		   mc.sent_entertainment);

	//printf("Total messages received:\n");
	printf("CONFORT ENTERTAINMENT ACCELERATE BREAK CALL_HELP\n");
	printf("%7d %13d %10d %5d %9d\n", mc.rcvd_confort, mc.rcvd_entertainment,
		   mc.rcvd_accelerate, mc.rcvd_break, mc.rcvd_call_help);
}

int main(int argc, char * argv[]){

	struct hostent *host_address;
	struct sockaddr_in socket_address, conf_address;
	char *host;
	char buf[MAX_LINE], client_ip[INET_ADDRSTRLEN];
	car *cars;
	message_buffer *msgbuf;
	unsigned addrlen;
	unsigned short client_port;
	int s, i, res;
	int n_cars, n_msgs;
	size_t len;
	int bytesreceived;
	struct timeval tv;
	long t;
	msg_counter mc;
	int numUpdate = 0;
	time_register tr;

	/* verificação de argumentos */
	if (argc != 3) {
		fprintf(stderr, "Numero de argumentos invalido\n");
		fprintf(stderr, "Use:./lateServer [hostname_fastServer] [numero_maximo_de_carros]\n");
		return -1;
	}

	host = argv[1];

	/* tradução de nome para endereço IP */
	host_address = gethostbyname(host);

	if (host_address == NULL) {
		fprintf(stderr, "Falha na resolucao do nome de servidor.\n");
		return -1;
	}

	printf("Consegui hostbyname\n");

	/* criação da estrutura de dados de endereço */
	bzero(&buf, MAX_LINE);
	bzero((char *)&socket_address, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	bcopy(host_address->h_addr, (char *)&socket_address.sin_addr, host_address->h_length);
	socket_address.sin_port = htons(SERVER_PORT);
	len = host_address->h_length;
	bcopy((char *)host_address->h_addr_list[0],
		  (char *)&socket_address.sin_addr.s_addr, len);

	/* criação de socket ativo*/
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		fprintf(stderr, "Falha na criacao de socket ativo.\n");
		return -1;
	}

	/* estabelecimento da conexão */
	if (connect(s, (const struct sockaddr*)&socket_address,
				sizeof(socket_address)) == -1) {
		fprintf(stderr, "Falha no estabelecimento da conexao.\n");
		return -1;
	}

	/* Imprime informacoes das portas e IPs do cliente */
	addrlen = sizeof(conf_address);
	res = getsockname(s, (struct sockaddr *) &conf_address, &addrlen);
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
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

	n_cars = atoi(argv[2]);
	n_msgs = 0;
	cars = (car *)calloc(n_cars, sizeof(car));
	msgbuf = (message_buffer *)calloc((MAX_MSG_CAR*n_cars), sizeof(message_buffer));

	/* monta estruturas de controle */
	buildMsgCounter(&mc);
	t = get_time();
	buildTimeRegister(&tr, t);

	while (1) {
		bytesreceived = recv(s, &buf, MAX_LINE, 0);
		//printf("bytesreceived: %d\n", bytesreceived);

		//timeout retorna SOCKET_ERROR
		if ((bytesreceived > 0) && (n_msgs < MAX_MSG_CAR*n_cars)){
			msgbuf[n_msgs].timestamp = get_time();
			memcpy(&msgbuf[n_msgs].msg, buf, sizeof(message));
			if (msgbuf[n_msgs].msg.TYPE == CONFORT) {
				mc.rcvd_confort++;
			} else if (msgbuf[n_msgs].msg.TYPE == ENTERTAINMENT) {
				mc.rcvd_entertainment++;
			}
			n_msgs++;
		} else if (bytesreceived == 0) {
      break;
    }

		t = get_time();
		for (i = 0; i < n_msgs; i++) {
			if (t - msgbuf[i].timestamp >= CLOUD_DELAY) {
				answer(s, msgbuf[i].msg, &mc);
				remove_msg(msgbuf, i--, n_msgs--);
			}
		}

		/* Imprime mensagens de update a cada 1s */
		if(isTime(UPDATE, t, &tr))
			printUpdate(numUpdate++, mc);
	}

	close(s);
	return 0;
}
