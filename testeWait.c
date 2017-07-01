#include <time.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#define CLOUD 0
#define SECURITY 1
#define ENTERTAINMENT 2
#define CONFORT 3
#define INTERVAL_SECURITY 50
#define INTERVAL_ENTERTAINMENT 3000
#define INTERVAL_CONFORT 2000

/* estrutura que guarda o horario da ultima mensagem de certo tipo */
typedef struct time_register {
	long last_entertainment;
	long last_confort;
	long last_security;
} time_register;

/* retorna o tempo atual em ms */
long get_time () {
	struct timespec time;
	long time_sec;
	long time_ms;
	time_register tregister;

	if (clock_gettime(CLOCK_REALTIME, &time))
		return 0;
	time_sec = time.tv_sec;
	time_ms = (long) (time.tv_nsec / 1.0e6);
	time_ms += (long) time_sec * (1000);

	return time_ms;
}

/* Constroi um registrador de horarios */
void buildTimeRegister(time_register *tr, long currentTime) {
	tr->last_entertainment = currentTime;
	tr->last_confort = currentTime;
	tr->last_security = currentTime;
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
	}

	return 0;
}

/* Envia mensagens se estiver dentro do intervalo especificado */
void sendMessages (long currentTime, time_register *tr) {
	if (isTime(ENTERTAINMENT, currentTime, tr))
		printf("Are you entertained?\n");
	
	if (isTime(CONFORT, currentTime, tr))
	    printf("You have a new notification on Facebook\n");
}


int main() {
	long time, initial_time, tlast_entertain, tlast_confort;
	int i;
	time_register tr;
	
	// horario inicial
	time = get_time();
	if (!time) {
		printf("ERROR: can't retrive time\n");
	}
	
	initial_time = time;
	buildTimeRegister(&tr, time);
	time++; // somente para entrar no loop, nao muda a execucao
	
	while(time - initial_time < 5000) {
		
		time = get_time();
		if (!time) {
			printf("ERROR: can't retrive time\n");
		}

		sendMessages(time, &tr);
	}

	return 0;
}
	
