// Luigi Miazzo 2023-06-06

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

int srv_child_n = 0;
pid_t *srv_childs;
int srv_maxChilds = 32;
FILE *logfile;

void handleError(char *msg) {
  dprintf(STDERR_FILENO, "%s\n", msg);
  if (logfile != NULL)
    fclose(logfile);
  if (srv_childs != NULL)
    free(srv_childs);
  exit(EXIT_FAILURE);
}

void client(char *logFileName) {

  while ((logfile = fopen(logFileName, "r")) == NULL)
    sleep(1);

  int srv_pid = 0;
  fscanf(logfile, "%d", &srv_pid);
  if (srv_pid <= 0)
    handleError("Errore lettura file di log");

  printf("[client] server: %d\n", srv_pid);
  int n_childs = 0;

  while (1) {
    char cmd = getchar();
    if (cmd == '+') {
      if (n_childs < 10) {
        kill(srv_pid, SIGUSR1);
        n_childs++;
      }
      printf("[client] %d\n", n_childs);
    } else if (cmd == '-') {
      if (n_childs > 0) {
        kill(srv_pid, SIGUSR2);
        n_childs--;
      }
      printf("[client] %d\n", n_childs);
    } else if (cmd == '\n') {
      while (n_childs > 0) {
        kill(srv_pid, SIGUSR2);
        n_childs--;
        printf("[client] %d\n", n_childs);
        sleep(1);
      }
      kill(srv_pid, SIGINT);
      break;
    }

    while (getchar() != '\n')
      ;
  }

  exit(EXIT_SUCCESS);
}

void serverHandleUSR1(int signo) {
  if (signo != SIGUSR1)
    handleError("Errore fatale");

  pid_t child_pid = fork();
  if (child_pid < 0)
    handleError("Errore fork");

  if (child_pid == 0) {
    // child
    fclose(logfile);
    free(srv_childs);
    pause();
    exit(EXIT_SUCCESS);
  }

  // parent

  // realloc memory if necessary
  if (srv_maxChilds == srv_child_n) {
    srv_maxChilds *= 2;
    pid_t *tmp = realloc(srv_childs, srv_maxChilds * sizeof(pid_t));
    if (tmp == NULL)
      handleError("Errore allocazione memoria");

    srv_childs = tmp;
  }

  srv_childs[srv_child_n] = child_pid;
  fprintf(logfile, "+%d\n", child_pid);
  printf("[server] +%d\n", child_pid);
  fflush(logfile);
  srv_child_n++;
}

void serverHandleUSR2(int signo) {
  if (signo != SIGUSR2)
    handleError("Errore fatale");

  if (srv_child_n <= 0) {
    fprintf(logfile, "-%d\n", 0);
    printf("[server] -%d\n", 0);
    fflush(logfile);
    return;
  }

  srv_child_n--;
  pid_t child_pid = srv_childs[srv_child_n];
  if (kill(child_pid, SIGKILL) == -1)
    handleError("Errore usccione figlio");

  fprintf(logfile, "-%d\n", child_pid);
  printf("[server] -%d\n", child_pid);
  fflush(logfile);
}

void serverHandleINT(int signo) {
  if (signo != SIGINT)
    handleError("Errore fatale");

  fprintf(logfile, "%d\n", srv_child_n);
  for (int i = 0; i < srv_child_n; i++)
    kill(srv_childs[i], SIGKILL);

  free(srv_childs);
  exit(EXIT_SUCCESS);
}

void server(char *logFileName) {

  logfile = fopen(logFileName, "r");
  if (logfile != NULL)
    handleError("File di log gia' esistente");
  logfile = fopen(logFileName, "w");
  if (logfile == NULL)
    handleError("Errore apertura file di log");

  fprintf(logfile, "%d\n", getpid());
  fflush(logfile);
  printf("[server:%d]\n", getpid());

  srv_childs = malloc(srv_maxChilds * sizeof(pid_t));
  if (srv_childs == NULL)
    handleError("Errore allocazione memoria");

  signal(SIGUSR1, serverHandleUSR1);
  signal(SIGUSR2, serverHandleUSR2);
  signal(SIGINT, serverHandleINT);

  while (1)
    pause();
}

int main(int argc, char **argv) {

  if (argc != 3) {
    dprintf(STDERR_FILENO, "Uso errato: %s <client | server> <logfile>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "client") == 0)
    client(argv[2]);
  else if (strcmp(argv[1], "server") == 0)
    server(argv[2]);

  dprintf(STDERR_FILENO, "Il secondo parametro deve essere client | server\n");
  exit(EXIT_FAILURE);
}
