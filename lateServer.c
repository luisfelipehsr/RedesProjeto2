/**************************
****** Cliente TCP *******
**************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#define CLOUD 0
#define SECURITY 1
#define ENTERTAINMENT 2
#define CONFORT 3
#define UPDATE 4
#define INTERVAL_SECURITY 50
#define INTERVAL_ENTERTAINMENT 20
#define INTERVAL_CONFORT 200
#define INTERVAL_UPDATE 1000
#define CAR_REPORT 0
#define ACCELERATE 1
#define STOP 2
#define CALL_RESCUE 3

#define MAX_MSG_CAR 5
#define SERVER_PORT 12345
#define MAX_LINE 256

/* Estrutura padrao de mensagem de conforto/lazer */
typedef struct confort {
	char[64] url;
  char[100] text;
} confort;

/* Estrutura padrao de mensagem de entretenimento */
typedef struct entertain {
	char[20] appName;
	char[20] data;
} entertain;

/*estrutura para armazenar carros*/
typedef struct car {
  int id; /*5*/
  int x, y; /*2 digitos cada*/
  int vel, dir, sent, tam; /*2, 1, 1, 2*/
} car;

/* Estrutura padrao de qualquer tipo de mensagem */
typedef struct message {
	char TYPE;
	char MODE;
	char data[254];
} message;

typedef struct message_buffer {
	message msg;
  long timestamp;
} message_buffer;

long get_time () {
	struct timespec time;
	long time_sec;
	long time_ms;

	if (clock_gettime(CLOCK_REALTIME, &time))
		return 0;
	time_sec = time.tv_sec;
	time_ms = (long) (time.tv_nsec / 1.0e6);
	time_ms += (long) time_sec * (1000);

	return time_ms;
}

void remove_msg(message_buffer *msgbuf, int i, int n_msgs) {
  int j;
  for (j = i; j < n_msgs; j++) {
    msgbuf[j].timestamp = msgbuf[j+1].timestamp;
    msgbuf[j].msg.TYPE = msgbuf[j+1].msg.TYPE;
    msgbuf[j].msg.MODE = msgbuf[j+1].msg.MODE;
    msgbuf[j].msg.data = msgbuf[j+1].msg.data;
  }
}

int main(int argc, char * argv[]){

  struct hostent *host_address;
  struct sockaddr_in socket_address;
  char *host;
  char buf[MAX_LINE], eco[MAX_LINE + 10];
  car *cars;
  message_buffer *msgbuf;
  int s, i;
  int len, n_cars, n_msgs;
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
  len = sizeof(socket_address);

  /* criação de socket ativo*/
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    fprintf(stderr, "Falha na criacao de socket ativo.\n");
    return -1;
  }

  printf("Socket Criado\n");

  /* estabelecimento da conexão */
  if (connect(s, (struct sockaddr*)&socket_address, len) == -1) {
    fprintf(stderr, "Falha no estabelecimento da conexao.\n");
    return -1;
  }

  printf("Conectado\n");

  getsockname(s, (struct sockaddr*)&socket_address, len);
  printf("\nInformacoes do socket local:\nIP: %s\nPorta: %d\n\n", inet_ntoa((struct in_addr)socket_address.sin_addr),socket_address.sin_port);

  tv.tv_sec = 0;
  tv.tv_usec = 20;
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval))

  n_cars = argv[2];
  n_msgs = 0;

  cars = (car *)calloc(n_cars, sizeof(car));
  msgbuf = (message_buffer *)calloc((MAX_MSG_CAR*n_cars), sizeof(message_buffer));

  while (1) {
    bytesreceived = recv(s, &buf, MAX_LINE, 0);

    if ((bytesreceived >= 0) && (n_msgs < MAX_MSG_CAR*n_cars)){
      msgbuf[n_msgs].timestamp = get_time();
      msgbuf[n_msgs].msg.TYPE = buf[0];
      msgbuf[n_msgs].msg.MODE = buf[1];
      msgbuf[n_msgs].msg.data = buf;
      n_msgs++;
    }

    t = get_time();
    for (i = 0; i < n_msgs; i++) {
      if (t - msgbuf[i].timestamp >= 200) {
        /* RESPONDER MENSAGEM */

        remove_msg(msgbuf, i--, n_msgs--);
      }
    }

    bytesreceived = -1;
  }

  close(s);
  return 0;
}
