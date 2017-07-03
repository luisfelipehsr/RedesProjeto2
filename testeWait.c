#include <time.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <sys/socket.h>

#define CLOUD 0
#define SECURITY 1
#define ENTERTAINMENT 2
#define CONFORT 3
#define UPDATE 4
#define INTERVAL_SECURITY 50
#define INTERVAL_ENTERTAINMENT 20
#define INTERVAL_CONFORT 1000
#define INTERVAL_UPDATE 1000
#define STOP_SIMULATION 5
#define STOP_TIME 20000
#define CAR_REPORT 0
#define ACCELERATE 1
#define STOP 2
#define CALL_RESCUE 3



/* Estrutura padrao de mensagem de conforto/lazer */
typedef struct confort {
	char url[64];
    char text[100];
} confort;

/* Estrutura padrao de mensagem de entretenimento */
typedef struct entertain {
	char appName[20];
	char data[200];
} entertain;

// PRECISA MEXER !!!!!!!!!!!!!!!!!!!!
typedef struct car {
	int x, y;
	int dir, vel;
} car;

/* Estrutura padrao de qualquer tipo de mensagem */
typedef struct message {
	char TYPE;
	char MODIFIER;
	char data[254];
} message;



/* estrutura que guarda o ultimo horario de certa acao e seu numero */
typedef struct time_register {
	time_t last_entertainment;
	time_t last_confort;
	time_t last_security;
	time_t last_update;
	time_t start_time;
	int numEntertainment;
	int numConfort;
	int numSecurity;
} time_register;

/* retorna o tempo atual em ms */
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

/* Constroi um registrador de horarios */
void buildTimeRegister(time_register *tr, long currentTime) {
	tr->last_entertainment = currentTime;
	tr->last_confort = currentTime;
	tr->last_security = currentTime;
	tr->last_update = currentTime;
	tr->start_time = currentTime;
	tr->numEntertainment = 0;
	tr->numConfort = 0;
	tr->numSecurity = 0;
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
	} else if (mode == STOP_SIMULATION &&
			   currentTime - tr->start_time > STOP_TIME) {
		return 1;
	}
	
	return 0;
}

/* Envia mensagens se estiver dentro do intervalo especificado */
void sendMessages (long currentTime, time_register *tr) {
	if (isTime(ENTERTAINMENT, currentTime, tr))
		//printf("Are you entertained?\n");
		tr->numEntertainment += 1;
	
	if (isTime(CONFORT, currentTime, tr))
	    //printf("You have a new notification on Facebook\n");
		tr->numConfort += 1;
}

void displayActivities(long currentTime, time_register *tr) {
	if (isTime(UPDATE, currentTime, tr)) {
		printf("UPDATE:\n%d entertainment messages sent\n",
			   tr->numEntertainment);
		printf("%d confort messages sent\n%d security messages sent\n",
			   tr->numConfort, tr->numSecurity);
		tr->numEntertainment = 0;
		tr->numConfort = 0;
		tr->numSecurity = 0;
	}
}



int main() {
	long time, initial_time;
	int i;
	time_register tr;
	
	// horario inicial
	time = get_time();
	if (!time) {
		printf("ERROR: can't retrive time\n");
	}

	// inicia variaveis
	initial_time = time;
	buildTimeRegister(&tr, time);
	time++; // somente para entrar no loop, nao muda a execucao

	// TESTES COM HEADER E DATA, mensagem de tamanho 256, 2 bytes de header
	printf("%ld\n", sizeof(struct message));
	char buffer[20];
	car c, d;
	char a, b;
	message m;
	c.x = 3; c.y = 4; c.dir = 1; c.vel = 64;
	m.TYPE = SECURITY;
	memcpy(&m.data, &c, sizeof(car));

	memcpy(&d, &m.data, sizeof(car));
	memcpy(&a, &m.TYPE, sizeof(char));
	memcpy(&b, &m.MODIFIER, sizeof(char));
 
	printf("TYPE: %d\nMODIFIER: %d\ndata: %d %d %d %d\n",
		   a, b, d.x, d.y, d.dir, d.vel);

	
	strcpy(buffer, "something written\n");
	printf("%s", buffer+4);

	
	// imprime atividades realizadas a cada certo intervalo
	while(!isTime(STOP_SIMULATION, time, &tr)) {
		
		time = get_time();
		if (!time) {
			printf("ERROR: can't retrive time\n");
		}

		sendMessages(time, &tr);
		displayActivities(time, &tr);
	}

	return 0;
}
	
