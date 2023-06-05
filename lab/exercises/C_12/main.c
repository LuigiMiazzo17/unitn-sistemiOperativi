#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MIN_CHILD_LIST_LEN 16

/*
 * Creare un’applicazione in C che gestisca un albero di processi tramite dei
 * segnali. In particolare, il programma, una volta lanciato, deve accettare i
 * seguenti comandi:
 * ● c n --> crea nuovi figli al livello n
 * ● k n --> termina i figli al livello n
 * ● p --> stampa in output l’albero dei processi
 * ● q --> termina il processo dopo aver terminato tutti i figli L’intera
 * comunicazione deve avvenire esclusivamente tramite segnali inviati dal
 * processo principale. L’output del comando ‘p’ non deve essere ordinato ma
 * deve essere ben chiaro il livello di ogni processo (per esempio usando la
 * tabulazione).
 */

pid_t *child_list;
int *child_level;
pid_t *parent_list;

void safeExit(const int _status) {
  if (child_list != NULL)
    free(child_list);
  if (child_level != NULL)
    free(child_level);
  if (parent_list != NULL)
    free(parent_list);
  exit(_status);
}

void errorHandle(const char *msg) {
  perror(msg);
  safeExit(1);
}

void printHelp() {
  printf("Gestore albero di figli con segnali\n");
  printf("Comandi disponibili:\n");
  printf("c n --> crea nuovi figli al livello n\n");
  printf("k n --> termina i figli al livello n\n");
  printf("p --> stampa in output l’albero dei processi\n");
  printf("q --> termina il processo dopo aver terminato tutti i figli\n");
}

void reallocateLists(const int maxChild) {
  pid_t *child_list_tmp = realloc(child_list, maxChild * sizeof(pid_t));
  if (child_list_tmp == NULL) {
    errorHandle("realloc");
  }
  int *child_level_tmp = realloc(child_level, maxChild * sizeof(int));
  if (child_level_tmp == NULL) {
    free(child_list_tmp);
    errorHandle("realloc");
  }

  pid_t *parent_list_tmp = realloc(parent_list, maxChild * sizeof(pid_t));
  if (parent_list_tmp == NULL) {
    free(child_list_tmp);
    free(child_level_tmp);
    errorHandle("realloc");
  }

  child_list = child_list_tmp;
  child_level = child_level_tmp;
  parent_list = parent_list_tmp;
}

int main(int argc, char *argv[]) {
  child_list = malloc(MIN_CHILD_LIST_LEN * sizeof(pid_t));
  if (child_list == NULL)
    errorHandle("malloc");

  child_level = malloc(MIN_CHILD_LIST_LEN * sizeof(int));
  if (child_level == NULL)
    errorHandle("malloc");

  parent_list = malloc(MIN_CHILD_LIST_LEN * sizeof(pid_t));
  if (parent_list == NULL)
    errorHandle("malloc");

  int n_child = 0;
  int maxChild = MIN_CHILD_LIST_LEN;

  child_list[0] = getpid();
  child_level[0] = 0;
  parent_list[0] = getpid();
  n_child++;

  while (1) {
    char cmd;
    int n;

    printf("Insert command: ");

    // read command
    scanf("%c", &cmd);
    if (cmd == '\n')
      continue;
    if (cmd == 'c' || cmd == 'k')
      scanf("%d", &n);

    // scanf("%c%d", &cmd, &n);
    // printf("Command: %c %d\n", cmd, n);
    // consume all cin
    while (getchar() != '\n')
      ;

    switch (cmd) {
    case 'c':
      // check if there is enough space in child_list
      if (n_child == maxChild) {
        maxChild *= 2;
        reallocateLists(maxChild);
      }

      // create childs at level n
      int c_added = 0;
      for (int i = 0; i < n_child; i++) {
        if (child_level[i] == n - 1) {

          pid_t pid = fork();
          if (pid == -1) {
            errorHandle("fork");
          }

          if (pid != 0) {
            // parent
            child_list[n_child] = pid;
            child_level[n_child] = n;
            parent_list[n_child] = child_list[i];
            n_child++;
            c_added++;
            printf("Added child at level %d with pid %d and ppid %d\n", n, pid,
                   child_list[i]);
            if (n_child == maxChild) {
              maxChild *= 2;
              reallocateLists(maxChild);
            }
          } else {
            // child
            pause();
            safeExit(0);
          }
        }
      }
      if (c_added == 0)
        printf("No child added\n");

      break;
    case 'k':

      // check if has sense to kill
      if (n_child == 0) {
        printf("No child to kill\n");
        break;
      } else if (n < 0) {
        printf("Invalid level\n");
        break;
      } else if (n == 0) {
        printf("Can't kill the root of the tree\n");
      }

      // check if there is at least 1 child of level n, if so, abort
      int hasToQuit = 0;
      for (int i = 0; i < n_child; i++)
        if (child_level[i] > n) {
          printf("Can't kill child because there is at least 1 child of level "
                 "%d\n",
                 n);
          hasToQuit = 1;
          break;
        }
      if (hasToQuit)
        break;

      // kill all child of level n
      int c_killed = 0;
      for (int i = 1; i < n_child; i++)
        if (child_level[i] == n) {
          printf("Killing child %d\n", child_list[i]);
          kill(child_list[i], SIGTERM);
          child_level[i] = -1;
          c_killed++;
        }
      if (c_killed == 0)
        printf("No child to kill!\n");

      // defragmenting child_list and child_level
      int j = 0;
      for (int i = 0; i < n_child; i++)
        if (child_level[i] != -1) {
          child_level[j] = child_level[i];
          child_list[j] = child_list[i];
          j++;
        }
      n_child = j;

      break;
    case 'p':

      for (int i = 0; i < n_child; i++) {
        for (j = 0; j < child_level[i]; j++)
          printf("\t");

        printf("Child with depth %d and pid %d and ppid %d\n", child_level[i],
               child_list[i], parent_list[i]);
      }

      break;
    case 'q':
      for (int i = 1; i < n_child; i++)
        kill(child_list[i], SIGTERM);

      while (wait(NULL) != -1)
        ;

      printf("All child terminated\n");
      printf("Bye!\n");
      safeExit(0);
      break;
    default:
      printHelp();
      break;
    }
  }
}
