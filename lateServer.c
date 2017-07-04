/***************************************
****** Servidor de alta latencia *******
****************************************/
#include "header.h"

#define MAX_MSG_CAR 5
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

int answer(int sockfd, message msg){
	char buf[MAX_LINE];
	message out;
	entertain ent;
	confort conf;
	int n;

  out.SENDTIME = msg.SENDTIME;

	srand((int) get_time());
	n=rand()%5;

	if (msg.TYPE == CONFORT) {
		memcpy(&conf, &msg.data, sizeof(confort));
		if (strcmp(conf.url, URL_FACEBOOK) == 0) {
			if (n < 3) {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "A pessoa que voce NAO quer pegar curtiu seu status: %s",
							conf.text);
			} else {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "A pessoa que voce QUER pegar curtiu seu status: %s",
							conf.text);
			}

			memcpy(buf, &out, sizeof(message));
		} else if (strcmp(conf.url, URL_TWITTER) == 0) {
			if (n < 3) {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "A pessoa que voce NAO quer pegar retweetou: %s",
							conf.text);
			} else {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "A pessoa que voce QUER pegar retweetou: %s",
							conf.text);
			}

			memcpy(buf, &out, sizeof(message));
		}

	} else if (msg.TYPE == ENTERTAINMENT) {
		memcpy(&ent, &msg.data, sizeof(entertain));
		if (strcmp(ent.appName, APP_TIBIA) == 0) {
			if (n < 3) {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "Outro jogador te matou e roubou suas coisas quando voce matou o orc\n");

			} else {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "Voce conseguiu derrotar o orc com sucesso! Congratz! GG!\n");
			}

			memcpy(buf, &out, sizeof(message));
		} else if (strcmp(ent.appName, APP_POKEMON) == 0) {
			if (n < 3) {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "The wild code fled...\n");

			} else {
				out.TYPE = msg.TYPE;
				out.MODIFIER = msg.MODIFIER;
				snprintf(out.data, sizeof(out.data), "You caught the wild code! You won %d code candies\n",
							n);
			}

			memcpy(buf, &out, sizeof(message));
		}
	}


	return send(sockfd, &buf, MAX_LINE, 0);
}


int main(int argc, char * argv[]){

  struct hostent *host_address;
  struct sockaddr_in socket_address, conf_address;
  char *host;
  char buf[MAX_LINE], eco[MAX_LINE + 10], client_ip[INET_ADDRSTRLEN];
  car *cars;
  message_buffer *msgbuf;
  unsigned addrlen;
  unsigned short client_port;
  int s, i, res;
  int n_cars, n_msgs;
  size_t len;
  int bytessent, bytesreceived;
  struct timeval tv;
  long t;

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
  //len = sizeof(socket_address);
  len = host_address->h_length;
  bcopy((char *)host_address->h_addr_list[0],
		(char *)&socket_address.sin_addr.s_addr, len);

  /* criação de socket ativo*/
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    fprintf(stderr, "Falha na criacao de socket ativo.\n");
    return -1;
  }

  //printf("Socket Criado\n");

  /* estabelecimento da conexão */
  if (connect(s, (const struct sockaddr*)&socket_address,
			  sizeof(socket_address)) == -1) {
    fprintf(stderr, "Falha no estabelecimento da conexao.\n");
    return -1;
  }
  /*
  printf("Conectado\n");

  addrlen = sizeof(conf_address);
  getsockname(s, (struct sockaddr*) &conf_address, &addrlen);
  printf("\nInformacoes do socket local:\nIP: %s\nPorta: %u\n\n", inet_ntoa((struct in_addr) conf_address.sin_addr),conf_address.sin_port);
  */

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

  strcpy(buf, "Servidor de altalatencia criado!\0");
  bytessent = send(s, &buf, MAX_LINE, 0);

  while (1) {
    bytesreceived = recv(s, &buf, MAX_LINE, 0);

    //timeout retorna SOCKET_ERROR
    if ((bytesreceived >= 0) && (n_msgs < MAX_MSG_CAR*n_cars)){
      msgbuf[n_msgs].timestamp = get_time();
      memcpy(&msgbuf[n_msgs].msg, buf, sizeof(message));
      n_msgs++;
    }

    t = get_time();
    for (i = 0; i < n_msgs; i++) {
      if (t - msgbuf[i].timestamp >= 2000) {
        bytessent = answer(s, msgbuf[i].msg);
        remove_msg(msgbuf, i--, n_msgs--);
      }
    }

  }

  close(s);
  return 0;
}
