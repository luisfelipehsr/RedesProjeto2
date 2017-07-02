/**************************
****** Cliente TCP *******
**************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define MAX_LINE 256

int main(int argc, char * argv[]){

  struct hostent *host_address;
  struct sockaddr_in socket_address;
  char *host;
  char buf[MAX_LINE], eco[MAX_LINE + 10];
  int s;
  int len;
  char lixo, comando = 'e';
  int bytessent, bytesreceived;

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

  /* ler e enviar linhas de texto, receber eco */
	while (1) {

    bytesreceived = recv(s, &eco, MAX_LINE+10, 0);

    

    bytessent = send(s, &buf, MAX_LINE, 0);
  }

  close(s);
  return 0;
}
