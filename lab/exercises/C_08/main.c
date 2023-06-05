#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

#define BUFF_SIZE 64

/*
 * Impostare una comunicazione bidirezionale tra due processi con due livelli di
 * complessità: Alternando almeno due scambi (P1 → P2, P2 → P1, P1 → P2, P2 →
 * P1)
 */

void errorHandle(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int ptoc[2], ctop[2];

  if (pipe(ptoc))
    errorHandle("pipe1");
  if (pipe(ctop))
    errorHandle("pipe2");

  int pidChild = fork();

  if (pidChild == 0) {
    // Parent
    close(ptoc[READ_END]);
    close(ctop[WRITE_END]);

    char buff[BUFF_SIZE] = "Hello, World!";
    write(ptoc[WRITE_END], buff, strlen(buff));
    printf("Parent sent: %s\n", buff);

    int bRead = read(ctop[READ_END], buff, sizeof(buff));
    buff[bRead - 1] = '\0'; // Removing last char of what was sent
    write(ptoc[WRITE_END], buff, strlen(buff));
    printf("Parent received and sent back: %s\n", buff);

    bRead = read(ctop[READ_END], buff, sizeof(buff));
    buff[bRead - 1] = '\0'; // Removing last char of what was sent
    printf("Parent received: %s\n", buff);

    close(ptoc[WRITE_END]);
    close(ctop[READ_END]);
  } else {
    // Child
    close(ctop[READ_END]);
    close(ptoc[WRITE_END]);

    char buff[BUFF_SIZE];

    for (int i = 0; i < 2; i++) {
      int bRead = read(ptoc[READ_END], buff, sizeof(buff));
      buff[bRead - 1] = '\0'; // Removing last char of what was sent
      write(ctop[WRITE_END], buff, strlen(buff));
      printf("Child received and sent back: %s\n", buff);
    }

    close(ctop[WRITE_END]);
    close(ptoc[READ_END]);
  }
  return 0;
}
