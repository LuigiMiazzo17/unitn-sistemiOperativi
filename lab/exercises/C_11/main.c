#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

#define BUFF_SIZE 64

/*
 * Creare un programma che prenda come argomento 'n' il numero di figli da
 * generare. Ogni figlio creato comunicherà al genitore (tramite pipe) un numero
 * casuale e il genitore calcolerà la somma di tutti questi numeri, inviandola a
 * stdout.
 */

void errorHandle(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <int>\n", argv[0]);
    return 1;
  }

  int n = atoi(argv[1]);
  int sum = 0;
  int ctop[2];

  if (pipe(ctop))
    errorHandle("pipe");

  for (int i = 0; i < n; i++) {
    pid_t last_pid = fork();

    if (last_pid == 0) {
      // Child
      close(ctop[READ_END]);
      // set seed
      srand(time(NULL) + getpid());
      int num = rand() % 101;
      printf("Child %d: %d\n", i, num);

      write(ctop[WRITE_END], &num, sizeof(num));
      close(ctop[WRITE_END]);

      return 0;
    } else {
      // Parent
      int buff;

      wait(NULL);

      int bRead = read(ctop[READ_END], &buff, sizeof(buff));
      if (bRead == -1)
        errorHandle("read");
      sum += buff;
    }
  }
  while (wait(NULL) > 0)
    ;

  close(ctop[READ_END]);
  close(ctop[WRITE_END]);
  printf("Sum: %d\n", sum);

  return 0;
}
