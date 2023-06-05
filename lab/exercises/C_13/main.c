#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MSG_SIZE 256

/*
 * Implementare in C una coda di messaggi basata sulla priorità, che consenta di
 * aggiungere messaggi con livelli di priorità diversi e di eliminarli in base
 * al loro livello di priorità.
 */

void errorHandle(const char *msg) {
  perror(msg);
  exit(1);
}

struct msgbuf {
  long mtype;
  char mtext[MAX_MSG_SIZE];
} sendbuf, recvbuf;

int main(int argc, char *argv[]) {

  creat("/tmp/unique", 0777);
  key_t queueKey = ftok("/tmp/unique", 1);
  if (queueKey == -1) {
    errorHandle("ftok");
  }

  printf("Do you want to keep old messages? (Y/n): ");
  char c = getchar();
  int queueId;
  if (c == 'n' || c == 'N') {
    queueId = msgget(queueKey, IPC_CREAT | IPC_EXCL | 0777);
    if (queueId > 0)
      msgctl(queueId, IPC_RMID, NULL);
  }

  queueId = msgget(queueKey, IPC_CREAT | 0777);

  if (queueId == -1) {
    errorHandle("msgget");
  }

  while (1) {
    printf("\nInsert command: ");
    char cmd = getchar();

    while (getchar() != '\n')
      ;

    switch (cmd) {
    case 'a':
      // adds a mesg to the queue
      printf("Insert priority: ");
      int priority;

      scanf("%d", &priority);
      while (getchar() != '\n')
        ;
      printf("Insert message: ");
      char buff[MAX_MSG_SIZE];
      fgets(buff, MAX_MSG_SIZE, stdin);
      buff[strlen(buff) - 1] = '\0';

      sendbuf.mtype = priority;
      sprintf(sendbuf.mtext, "%s", buff);
      if (msgsnd(queueId, &sendbuf, sizeof(sendbuf.mtext), 0)) {
        errorHandle("msgsnd");
      }
      break;
    case 'r':
      // removes a all mesgs with a given priority
      printf("Insert priority: ");
      int priority2;
      scanf("%d", &priority2);
      while (getchar() != '\n')
        ;
      while (msgrcv(queueId, &recvbuf, sizeof(recvbuf.mtext), priority2,
                    IPC_NOWAIT) != -1) {
        printf("Message: %s\n", recvbuf.mtext);
      }

      break;
    case 'q':
      printf("Delete queue? (Y/n): ");
      c = getchar();
      if (c != 'n' && c != 'N')
        msgctl(queueId, IPC_RMID, NULL);
      printf("Bye bye\n");
      exit(0);
      break;
    default:
      break;
    }
  }
}
