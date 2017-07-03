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
#include <sys/socket.h>
#include <netinet/in.h>

#define CLOUD 0
#define SECURITY 1
#define ENTERTAINMENT 2
#define CONFORT 3
#define UPDATE 4
#define STOP 5
#define INTERVAL_SECURITY 50
#define INTERVAL_ENTERTAINMENT 20
#define INTERVAL_CONFORT 2000
#define INTERVAL_UPDATE 1000
#define SIMULATION_DURATION 20000
#define CAR_REPORT 0
#define ACCELERATE 1
#define STOP_TIME 2
#define CALL_RESCUE 3
#define URL_FACEBOOK "www.facebook.com"
#define URL_TWITTER "www.twitter.com"
#define APP_TIBIA "tibia"
#define APP_POKEMON "pokemon"

/* Estrutura padrao de mensagem de conforto/lazer */
typedef struct confort {
	char url[64];
    char text[100];
} confort;

/* Estrutura padrao de mensagem de entretenimento */
typedef struct entertain {
	char appName[20];
	char data[60];
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


/* estrutura que guarda o ultimo horario de certa acao e seu numero */
typedef struct time_register {
	time_t last_entertainment;
	time_t last_confort;
	time_t last_security;
	time_t last_update;
	time_t start_time;
} time_register;

/* Constroi um registrador de horarios */
void buildTimeRegister(time_register *tr, long currentTime) {
	tr->last_entertainment = currentTime;
	tr->last_confort = currentTime;
	tr->last_security = currentTime;
	tr->last_update = currentTime;
	tr->start_time = currentTime;
}


/* Determina se esta no horario de algo de acordo com um intervalo */
int isTime (int mode, long currentTime, time_register *tr) {
	if (mode == ENTERTAINMENT &&
		currentTime - tr->last_entertainment > INTERVAL_ENTERTAINMENT) {
	    tr->last_entertainment = currentTime;
		return 1;
	} else if (mode == CONFORT &&
			   currentTime - tr->last_confort > INTERVAL_CONFORT) {
		tr->last_confort = currentTime;
		return 1;
	} else if (mode == UPDATE &&
			   currentTime - tr->last_update > INTERVAL_UPDATE) {
		tr->last_update = currentTime;
		return 1;
	} else if (mode == STOP_TIME &&
			   currentTime - tr->start_time > SIMULATION_DURATION) {
		return 1;
	}
	
	return 0;
}


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
