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
  tv.tv_usec = 2000; // == 2 ms
  //timeout de 2ms para o recv
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval))

  n_cars = argv[2];
  n_msgs = 0;

  cars = (car *)calloc(n_cars, sizeof(car));
  msgbuf = (message_buffer *)calloc((MAX_MSG_CAR*n_cars), sizeof(message_buffer));

  strcpy(buf, "Servidor de altalatencia criado!\0");
  bytessent = send(s, &buf, MAX_LINE, 0);

  while (1) {
    bytesreceived = recv(s, &buf, MAX_LINE, 0);

    //timeout retorna SOCKET_ERROR
    if ((bytesreceived != SOCKET_ERROR) && (n_msgs < MAX_MSG_CAR*n_cars)){
      msgbuf[n_msgs].timestamp = get_time();
      msgbuf[n_msgs].msg.TYPE = buf[0];
      msgbuf[n_msgs].msg.MODE = buf[1];
      msgbuf[n_msgs].msg.data = buf;
      n_msgs++;
    }

    t = get_time();
    for (i = 0; i < n_msgs; i++) {
      if (t - msgbuf[i].timestamp >= 2000) {
        /* RESPONDER MENSAGEM */
        if (msgbuf[i] == ENTERTAINMENT) {
          strcpy(buf, "Jogo para quem esta dirigindo recebeu msg\0");
          bytessent = send(s, &buf, MAX_LINE, 0);
        } else if (msgbuf[i] == CONFORT) {
          strcpy(buf, "Pessoa que voce nao vai pegar postou foto\0");
          bytessent = send(s, &buf, MAX_LINE, 0);
        }

        /*ver como definir carro destinatario/rememtente*/

        remove_msg(msgbuf, i--, n_msgs--);
      }
    }

  }

  close(s);
  return 0;
}
