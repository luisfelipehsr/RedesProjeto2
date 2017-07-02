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
