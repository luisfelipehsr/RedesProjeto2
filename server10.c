
#define COL_LIMIT 5

typedef struct message {
  int timestamp; /*10 digitos*/
  int id; /*5*/
  int x, y; /*2 digitos cada*/
  int vel, dir, sent, tam; /*2, 1, 1, 2*/
} message;

typedef struct car {
  int id; /*5*/
  int x, y; /*2 digitos cada*/
  int vel, dir, sent, tam; /*2, 1, 1, 2*/
} car;

message strtomsg (char* str){
  message msg;

  char substr[11];
  memcpy(substr, &str[0], 10);
  substr[11] = '\0';
  msg.timestamp = atoi(substr);

  memcpy(substr, &str[10], 5);
  substr[5] = '\0';
  msg.id = atoi(substr);

  memcpy(substr, &str[15], 2);
  substr[2] = '\0';
  msg.x = atoi(substr);

  memcpy(substr, &str[17], 2);
  substr[2] = '\0';
  msg.y = atoi(substr);

  memcpy(substr, &str[19], 2);
  substr[2] = '\0';
  msg.vel = atoi(substr);

  memcpy(substr, &str[21], 1);
  substr[1] = '\0';
  msg.dir = atoi(substr);

  memcpy(substr, &str[22], 1);
  substr[1] = '\0';
  msg.sent = atoi(substr);

  memcpy(substr, &str[23], 2);
  substr[2] = '\0';
  msg.tam = atoi(substr);

  return msg;
}

char* msgtostr(message msg){
  char* str[26];

  sprintf(str, "%010d%05d%02d%02d%02d%01d%01d%02d", msg.timestamp, masg.id, msg.x, msg.y, msg.vel, msg.dir, msg.sent, msg.tam);

  return str;
}

int* checkcolision(car cars[], int n){
  int tempo_ii, tempo_ij, tempo_fi, tempo_fj;

  for (size_t i = 0; i < n-1; i++) {
    for (size_t j = i+1; j < n; j++) {
      if(cars[i].dir != cars[j].dir){
        if (((cars[i].dir == 1)&&(cars[i].y*cars[i].sent < 0)&&(cars[j].x*cars[j].sent < 0)) ||
            ((cars[i].dir == 0)&&(cars[i].x*cars[i].sent < 0)&&(cars[j].y*cars[j].sent < 0))) {
          tempo_ii = cars[i].x / cars[i].vel;
          tempo_ij = cars[j].x / cars[j].vel;
          tempo_fi = tempo_ii + (cars[i].tam / cars[i].vel);
          tempo_fj = tempo_ij + (cars[j].tam / cars[j].vel);

          if (tempo_ii - tempo_ij < COL_LIMIT) || (tempo_ij - tempo_ii < COL_LIMIT) {
            /* chamar ambulancia */
          } else if ((tempo_ij >= tempo_ii) && (tempo_ij <= tempo_fi)) {
            /* j freia, i acelera */
          } else if ((tempo_ii >= tempo_ij) && (tempo_ii <= tempo_fj)) {
            /* i freia, j acelera */
          }
        }
      }
    }
  }

  return "\0";
}
