/****************************************
****** Servidor de baixa latencia *******
*****************************************/
#include "header.h"

#define LISTEN_PORT 12345
#define SERVER_PORT 23456
#define MAX_PENDING 5
#define MAX_LINE 256

int main(int argc, char * argv[]){

  struct sockaddr_in socket_address, cliaddr;
  char buf[MAX_LINE], eco[MAX_LINE+10];
  unsigned int len;
  int i, sp, sa, new_s, sockfd, cliente_num, maxfd, nready, clientes[FD_SETSIZE];
  fd_set todos_fds, novo_set;
  int bytessent, bytesreceived;

/*****************************************************************************/
/*****************************************************************************/

  if (argc != 2) {
    fprintf(stderr, "Numero de argumentos invalido\n");
    fprintf(stderr, "Use:./fastServer [numero_maximo_de_carros]\n");
    return -1;
	}

  /* criação da estrutura de dados de endereço */
  bzero((char *)&cliaddr, sizeof(cliaddr));
	bzero(&buf, MAX_LINE);

  len = sizeof(socket_address);
  bzero((char *)&socket_address, sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_addr.s_addr = INADDR_ANY;
  socket_address.sin_port = htons(LISTEN_PORT);

  /* criação de socket passivo */
	sp = socket(AF_INET, SOCK_STREAM, 0);
  if (sp == -1) {
    fprintf(stderr, "Falha na criacao de socket passivo.\n");
    return -1;
  }

	/* Associar socket ao descritor */
	if(bind(sp, (struct sockaddr*)&socket_address, len) == -1)  {
    fprintf(stderr, "Falha na atribuicao de endereco ao socket.\n");
    return -1;
  }

  /* Criar escuta do socket para aceitar conexões */
  if (listen (sp, MAX_PENDING) == -1) {
    fprintf(stderr, "Falha na criacao da escuta do socket.\n");
    return -1;
  }

  maxfd = sp;
  cliente_num = -1;
  for (i = 0; i < FD_SETSIZE; i++)
    clientes[i] = -1;

  FD_ZERO(&todos_fds);
  FD_SET(sp, &todos_fds);

  /* aguardar/aceitar conexão, receber e imprimir texto na tela, enviar eco */
  while(1) {
    novo_set = todos_fds;
    nready = select(maxfd+1, &novo_set, NULL, NULL, NULL);
    if(nready < 0) {
      perror("select" );
      exit(1);
    }

    if(FD_ISSET(sp, &novo_set)) {
      len = sizeof(socket_address);
      if ((new_s = accept(sp, (struct sockaddr *)&socket_address, &len)) < 0) {
        perror("simplex-talk: accept");
        exit(1);
      }

      getpeername(new_s, (struct sockaddr*)&cliaddr, &len);
      printf("NOVA CONEXAO ACEITA:\nInformacoes do socket remoto:\nIP: %s\nPorta: %d\n\n", inet_ntoa((struct in_addr)cliaddr.sin_addr),cliaddr.sin_port);

      printf("Esperando mensagem...\n\n");

      for (i = 0; i < FD_SETSIZE; i++) {
        if (clientes[i] < 0) {
          clientes[i] = new_s; 	//guarda descritor
          break;
        }
      }

      if (i == FD_SETSIZE) {
        perror("Numero maximo de clientes atingido.");
   			exit(1);
      }

      FD_SET(new_s, &todos_fds);		// adiciona novo descritor ao conjunto
      if (new_s > maxfd)
        maxfd = new_s;			// para o select
      if (i > cliente_num)
        cliente_num = i;		// índice máximo no vetor clientes[]
      if (--nready <= 0)
        continue;			// não existem mais descritores para serem lidos
    }

    for (i = 0; i <= cliente_num; i++) {	// verificar se há dados em todos os clientes
        if ( (sockfd = clientes[i]) < 0)
          continue;
        if (FD_ISSET(sockfd, &novo_set)) {
          if ( (len = recv(sockfd, buf, sizeof(buf), 0)) == 0) {
            //conexão encerrada pelo cliente
            getpeername(sockfd, (struct sockaddr*)&cliaddr, &len);
            printf("IP %s DESCONECTADO!\n", inet_ntoa((struct in_addr)cliaddr.sin_addr));

            close(sockfd);
            FD_CLR(sockfd, &todos_fds);
            clientes[i] = -1;
          } else {
            /* imprime ip e porta do cliente e envia texto de volta */
            getpeername(sockfd, (struct sockaddr*)&cliaddr, &len);

            strcpy(eco, "EC0Oo: ");
            strcat(eco, buf);

            printf("MENSAGEM RECEBIDA:\nIP %s: %s\n", inet_ntoa((struct in_addr)cliaddr.sin_addr), buf);

            bytessent = send(new_s, &eco, MAX_LINE+10, 0);

            printf("Esperando mensagem...\n\n");

          }
          if (--nready <= 0)
            break;				// não existem mais descritores para serem lidos
        }
    }
  }

  close(new_s);
  close(sp);
  close(sa);
  return 0;
}
