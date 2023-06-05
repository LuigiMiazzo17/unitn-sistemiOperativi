#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

#define BUFF_SIZE 64
#define MSG_TERM "Hello"

/*
 * Impostare una comunicazione bidirezionale tra due processi con due livelli di
 * complessità: Alternando almeno due scambi (P1 → P2, P2 → P1, P1 → P2, P2 →
 * P1)
 * Estendendo il caso a mo’ di “ping-pong”, fino a un messaggio convenzionale
 * di “fine comunicazione”
 */

void errorHandle(const char *msg) {
  perror(msg);
  exit(1);
}

void rwLoop(int readFd, int writeFd, char *whoami) {
  char buff[BUFF_SIZE];
  while (1) {
    int bRead = read(readFd, buff, sizeof(buff));
    buff[bRead] = '\0';

    // if (strcmp(buff, MSG_TERM) == 0) {
    //   printf("%s received termination string: \"%s\"\n", whoami, MSG_TERM);
    //   break;
    // }

    if (strcmp(buff, MSG_TERM) == 0) {
      printf("%s received and sent back termination string: \"%s\"\n", whoami,
             buff);
      sleep(1);
      // write(writeFd, MSG_TERM, strlen(MSG_TERM));
      write(writeFd, buff, strlen(buff));
      break;
    }

    buff[bRead - 1] = '\0'; // Removing last char of what was sent
    sleep(1);
    write(writeFd, buff, strlen(buff));
    printf("%s received and sent back: \"%s\"\n", whoami, buff);
  }
  return;
}

int main(int argc, char **argv) {
  int ptoc[2], ctop[2];

  if (pipe(ptoc))
    errorHandle("pipe1");
  if (pipe(ctop))
    errorHandle("pipe2");

  pid_t pidChild = fork();
  if (pidChild == -1)
    errorHandle("fork");

  if (pidChild == 0) {
    // Parent
    close(ptoc[READ_END]);
    close(ctop[WRITE_END]);

    char *buff = "Hello, World!";
    write(ptoc[WRITE_END], buff, strlen(buff));
    printf("Parent sent: \"%s\"\n", buff);

    rwLoop(ctop[READ_END], ptoc[WRITE_END], "Parent");

    close(ptoc[WRITE_END]);
    close(ctop[READ_END]);
  } else {
    // Child
    close(ctop[READ_END]);
    close(ptoc[WRITE_END]);

    rwLoop(ptoc[READ_END], ctop[WRITE_END], "Child");

    close(ctop[WRITE_END]);
    close(ptoc[READ_END]);
  }

  while (wait(NULL) > 0)
    ;
  return 0;
}
