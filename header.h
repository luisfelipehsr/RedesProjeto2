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
#define STOP_SIMULATION 5
#define INTERVAL_SECURITY 200
#define INTERVAL_ENTERTAINMENT 20
#define INTERVAL_CONFORT 2000
#define INTERVAL_UPDATE 1000
#define INTERVAL_WAIT 5000
#define STOP_TIME 20000
#define CAR_REPORT 0
#define ACCELERATE 1
#define BREAK
#define CALL_RESCUE 3
#define VEL_WAIT_LIMIT 45
#define URL_FACEBOOK "www.facebook.com"
#define URL_TWITTER "www.twitter.com"
#define APP_TIBIA "tibia"
#define APP_POKEMON "pokemon"

/* estrutura para guardar contadores de numero de mensagens */
typedef struct msg_counter {
	int sent_security;
	int sent_confort;
	int sent_entertainment;
	int sent_accelerate;
	int sent_break;
	int rcvd_security;
	int rcvd_confort;
	int rcvd_entertainment;
	int rcvd_accelerate;
	int rcvd_break;
	int rcvd_call_help;
} msg_counter;

/*estrutura para armazenar carros*/
typedef struct car {
  int id; /*5*/
  int x, y; /*2 digitos cada*/
  int vel, dir, sent, tam; /*2, 1, 1, 2*/
} car;

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

/* Estrutura padrao de mensagem de seguranca */
typedef struct security {
	car carInfo;
} security;

/* Estrutura padrao de qualquer tipo de mensagem */
typedef struct message {
	time_t SENDTIME;
	char TYPE;
	char MODIFIER;
	char data[246];
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

void buildMsgCounter(msg_counter *mc) {
	mc->sent_security = 0;
	mc->sent_confort = 0;
	mc->sent_entertainment = 0;
	mc->sent_accelerate = 0;
	mc->sent_break = 0;
	mc->rcvd_security = 0;
	mc->rcvd_confort = 0;
	mc->rcvd_entertainment = 0;
	mc->rcvd_accelerate = 0;
	mc->rcvd_break = 0;
	mc->rcvd_call_help = 0;
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
	} else if (mode == SECURITY &&
			   currentTime - tr->last_security > INTERVAL_SECURITY) {
		tr->last_security = currentTime;
		return 1;
	} else if (mode == STOP_SIMULATION &&
			   currentTime - tr->start_time > STOP_TIME) {
		return 1;
	}

	return 0;
}


long get_time () {
	struct timespec time;
	long time_sec;
	long time_ms;

	if (clock_gettime(CLOCK_REALTIME, &time)) return 0;

	time_sec = time.tv_sec;
	time_ms = (long) (time.tv_nsec / 1.0e6);
	time_ms += (long) time_sec * (1000);

	return time_ms;
}
