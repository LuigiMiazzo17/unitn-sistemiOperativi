#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 64
#define FIFO_PTOC_LOCATION "/tmp/fifo_ptoc_C_10"
#define FIFO_CTOP_LOCATION "/tmp/fifo_ctop_C_10"

/*
 * Impostare una comunicazione bidirezionale tra due processi con due livelli di
 * complessità: Alterna due scambi (P1 → P2, P2 → P1, P1 → P2, P2 → P1)
 */

void errorHandle(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {

  if (unlink(FIFO_PTOC_LOCATION) < 0)
    errorHandle("unlink");

  if (unlink(FIFO_CTOP_LOCATION) < 0)
    errorHandle("unlink");

  if (mkfifo(FIFO_PTOC_LOCATION, S_IRUSR | S_IWUSR))
    errorHandle("mkfifo");

  if (mkfifo(FIFO_CTOP_LOCATION, S_IRUSR | S_IWUSR))
    errorHandle("mkfifo");

  pid_t pidChild = fork();
  if (pidChild == -1)
    errorHandle("fork");

  if (pidChild == 0) {
    // Parent

    int ptoc = open(FIFO_PTOC_LOCATION, O_WRONLY);
    int ctop = open(FIFO_CTOP_LOCATION, O_RDONLY);

    char buff[BUFF_SIZE] = "Hello, World!";
    write(ptoc, buff, strlen(buff));
    printf("Parent sent: %s\n", buff);

    int bRead = read(ctop, buff, sizeof(buff));
    buff[bRead - 1] = '\0'; // Removing last char of what was sent
    write(ptoc, buff, strlen(buff));
    printf("Parent received and sent back: %s\n", buff);

    bRead = read(ctop, buff, sizeof(buff));
    buff[bRead - 1] = '\0'; // Removing last char of what was sent
    printf("Parent received: %s\n", buff);

    close(ptoc);
    close(ctop);
  } else {
    // Child

    int ptoc = open(FIFO_PTOC_LOCATION, O_RDONLY);
    int ctop = open(FIFO_CTOP_LOCATION, O_WRONLY);

    char buff[BUFF_SIZE];

    for (int i = 0; i < 2; i++) {
      int bRead = read(ptoc, buff, sizeof(buff));
      buff[bRead - 1] = '\0'; // Removing last char of what was sent
      write(ctop, buff, strlen(buff));
      printf("Child received and sent back: %s\n", buff);
    }

    close(ctop);
    close(ptoc);
  }

  while (wait(NULL) > 0)
    ;
  return 0;
}
