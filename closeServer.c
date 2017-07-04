#include "header.h"

#define LISTEN_PORT 12345
#define MAX_LINE 256
#define MAX_PENDING 5
#define MAX_TOTAL_CONNECTIONS 6

#define COMMAND_EXIT 1
#define COMMAND_MSG 0

/* Recebe um comando do cliente. 
 * Retorna: -1 se um erro ocorreu, 0 se o cliente enviou um comando de exit ou
 *          1 se o cliente enviou uma linha de texto.
 */
int receive_command(int clientfd, char *buffer, size_t buffer_size) {
	int res, i;
	struct sockaddr_in client_address;
	char client_ip[INET_ADDRSTRLEN];
	unsigned addrlen;
	unsigned short client_port;
	message m;
	
	bzero(buffer, buffer_size); // limpa o buffer

	/* recebe e imprime o comando */
	res = recv(clientfd, buffer, buffer_size, 0);
	if (res == -1) {
		printf("ERROR: Couldn't receive from buffer\n");
		return -1;
	}

	memcpy(&m, &buffer, buffer_size);

	for(i = 0; i < 40; i++) {
		printf("%c ", buffer[i]);
	}
	
    printf("\nReceived: %s %c %c", m.SENDTIME, m.MODIFIER, m.TYPE);

	/* Imprime IP do cliente que enviou a mensagem */
	addrlen = sizeof(client_address);
	res = getpeername(clientfd, (struct sockaddr *) &client_address, &addrlen);
	client_port = htons(client_address.sin_port);
	if (res == -1) {
		printf("erro ao obter endereço do cliente\n");
		return 0;
	}
	inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
	printf("---------\nMensagem enviada por: %s:%u\n", client_ip, client_port);
	
	/* verifica se eh um comando de saida */
	//res = strcmp(buffer, "exit\n");
	//if (res == COMMAND_EXIT)
	//	return COMMAND_EXIT;

	return COMMAND_MSG;
}

/* Funcao principal. Ignora argumentos */
int main() {

	struct sockaddr_in socket_address, client_address, conf_address;
	char buf[MAX_LINE], client_ip[INET_ADDRSTRLEN];
	int listenfd, clientfd, res, hasCommands, pid, total_connections, n_ready;
	int maximumfd, maximumfd_index, clients[FD_SETSIZE], i, j;
    fd_set read_set, all_fds;
	unsigned int addrlen;
	unsigned short client_port;

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

	/* inicializa o controle e a própria lista de clientes */
	maximumfd = listenfd;
	maximumfd_index = -1;
	for (i = 0; i < FD_SETSIZE; i++)
		clients[i] = -1;

	FD_ZERO(&all_fds);
	FD_SET(listenfd, &all_fds);
	
	total_connections = 0;
    /* Aguarda e aceita ate 5 conexoes simultaneas.
       A conexao #MAX_TOTAL_CONNECTIONS eh condicao de parada do server */
	while (total_connections < MAX_TOTAL_CONNECTIONS) {

		/* usa read_set para saber os clientes com dados para leitura */
		read_set = all_fds;
		n_ready = select(maximumfd+1, &read_set, NULL,NULL,NULL);

		if (n_ready < 0) {
			printf("ERROR Couldn't use select\n");
			exit(1);
		}

		/* verifica nova conexao com FD_ISSET sobre o socket de listen */
		if(FD_ISSET(listenfd, &read_set)) {
			addrlen = sizeof(client_address);
			clientfd = accept(listenfd,(struct sockaddr *) &client_address,
							  &addrlen);
			if (clientfd == -1) {
				printf("ERROR: Couldn't accept connection\n");
				exit(1);
			}

			/* procura um slot na lista de clientes vazio */
			res = 0;
			i = -1;
			while (res == 0) {
				i++;
				if(clients[i] < 0) {
					clients[i] = clientfd;
					res = 1;
				}	
			}

			if (i == FD_SETSIZE) {
				printf("ERROR Numero maximo de clientes atingido.\n");
				exit(1);
			}
			
			/* Imprime informacoes das portas e IPs do cliente */
			addrlen = sizeof(conf_address);
			res = getpeername(clientfd, (struct sockaddr *) &conf_address,
							  &addrlen);
			if (res == -1) {
				printf("erro ao obter endereço do cliente\n");
				return 0;
			 
			} 
			inet_ntop(AF_INET, &(conf_address.sin_addr), client_ip,
					  INET_ADDRSTRLEN);
			client_port = htons(conf_address.sin_port);
			printf("Um cliente se conectou: %s:%u\n", client_ip,
				   client_port);
			total_connections++;
			FD_SET(clientfd, &all_fds); // adiciona novo cliente ao conjunto

			/* incrementa o valor maximo de descritor se preciso */
			if (clientfd > maximumfd)
				maximumfd = clientfd;
			if (i > maximumfd_index)
				maximumfd_index = i;
				
			if (--n_ready <= 0)
				continue;
		}

		/* adiciona dados dos clientes de acordo com read_set */			
		for (i = 0; i <= maximumfd_index; i++) {
			clientfd = clients[i];

			/* se o cliente existe neste indice, recebe mensagem */
			if (clientfd >= 0) {
				if (FD_ISSET(clientfd, &read_set)) {

					/* Recebe comandos do cliente. 
					 * Se mensagem, imprime na tela e envia eco.
					 * Se for um exit, termina a aplicacao */
					res = receive_command(clientfd, buf, MAX_LINE);

					buf[17] = BREAK;
					
				    if(send(clientfd, buf, MAX_LINE-1, 0) == -1)
						printf("ERROR: Couldn't send message to client %d\n",
							   i);
				}
			}
		}
	} // fim while total_connections

	printf("End condition reached, maximum total number of connections is %d.",
		   MAX_TOTAL_CONNECTIONS);
	printf(" Server disabled\n");
	
	/* Fecha descritor do socket de listen e de todos os accept*/
	for (i = 0; i < MAX_TOTAL_CONNECTIONS; i++) {
		clientfd = clients[i];
		if (clientfd >= 0)
			close(clientfd);
	}
	close(listenfd);
	
	return 0;
}
