#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 12345
#define MAX_LINE 256
#define COMMAND_MSG 1
#define COMMAND_EXIT 0

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

/* Funcao principal. Precisa de 1 e somente 1 argumento, o nome do servidor */
int main (int argc, char* argv[]) {
	
	struct hostent *host_address;
	struct sockaddr_in socket_address, conf_address;
	const char *host;
	char buf[MAX_LINE], client_ip[INET_ADDRSTRLEN];
	int hasCommands, command, response, socketfd, res;
	unsigned addrlen;
	unsigned short client_port;
	size_t len;
	
	/* verificação de argumentos */
	if (argc == 2)
		host = argv[1];
	else {	
		printf("ERROR: One argument should be provided. %d given.\n", argc-1);
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
	
	/* ler e enviar linhas de texto, receber eco */
	hasCommands = 1;
	while (hasCommands) {
		command = user_command(socketfd, buf, MAX_LINE);
	
		if (command == COMMAND_MSG) {
			response = server_response(socketfd, buf, MAX_LINE);

			if (response == -1) // caso de erro
				hasCommands = 0;
			
		} else { // caso de erro ou comando de saida
			hasCommands = 0;
		}
	}

	/* fecha descritor */
	close(socketfd);
    
	return 0;
}

	
