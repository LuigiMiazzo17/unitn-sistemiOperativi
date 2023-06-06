// Luigi Miazzo 2023-06-06

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

#define SUCCESS_EXIT 0
#define ARGC_FAILURE 3
#define N_FAILURE 4
#define TARGET_FAILURE 5
#define GENERIC_FAILURE 6
#define TEST_PID 75625

pid_t childs[10];
int nActiveChilds;

void killManager(int sig, siginfo_t *info, void *context) {
  union sigval value;
  value.sival_int = info->si_pid;

  while (nActiveChilds > 0) {
    // if (sigqueue(childs[nActiveChilds - 1], SIGUSR1, value) == -1) {
    //   dprintf(STDERR_FILENO, "Errore invio SIGUSR1 ad un figlio\n");
    //   exit(GENERIC_FAILURE);
    // }
    //
    if (kill(childs[nActiveChilds - 1], SIGUSR1) == -1) {
      dprintf(STDERR_FILENO, "Errore invio SIGUSR1 ad un figlio\n");
      exit(GENERIC_FAILURE);
    }
    nActiveChilds--;
  }
  exit(SUCCESS_EXIT);
}

void killChild(int sig, siginfo_t *info, void *context) {
  union sigval value;
  value.sival_int = info->si_pid;

  // sarebbe bello se esistesse sigqueue in MACOS

  // if (sigqueue(childs[nActiveChilds - 1], SIGUSR1, value) == -1) {
  //   dprintf(STDERR_FILENO, "Errore invio SIGUSR1 ad un figlio\n");
  //   exit(GENERIC_FAILURE);
  // }

  if (kill(childs[nActiveChilds - 1], SIGUSR1) == -1) {
    dprintf(STDERR_FILENO, "Errore invio SIGUSR1 ad un figlio\n");
    exit(GENERIC_FAILURE);
  }
  nActiveChilds--;
}

void childRecvdKill(int sigNum, siginfo_t *info, void *empty) {
  // if (info->si_value.sival_int <= 0) {
  //   dprintf(STDERR_FILENO, "Errore ricezione pid\n");
  //   exit(GENERIC_FAILURE);
  // }

  // kill(info->si_value.sival_int, SIGUSR2);
  kill(TEST_PID, SIGUSR2);
  exit(SUCCESS_EXIT);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    dprintf(STDERR_FILENO, "Numero di argomenti invalido\n");
    dprintf(STDERR_FILENO, "Uso: %s <target> <n>\n", argv[0]);
    exit(ARGC_FAILURE);
  }

  int n = atoi(argv[2]);
  if (n <= 0 || n > 10) {
    dprintf(STDERR_FILENO,
            "Valore <n> invalido, deve essere compreso tra 1 e 10\n");
    exit(N_FAILURE);
  }

  char *fileName = argv[1];
  FILE *file;
  file = fopen(fileName, "r");
  if (file == NULL) {
    file = fopen(fileName, "a");
    if (file == NULL) {
      dprintf(STDERR_FILENO, "Path invalido\n");
      exit(TARGET_FAILURE);
    }
  } else {
    dprintf(STDERR_FILENO, "Il file esiste gia'\n");
    exit(TARGET_FAILURE);
  }

  fprintf(file, "%d\n", getpid());
  fflush(file);

  pid_t child_pid = fork();
  if (child_pid == 0) {
    // manager
    fprintf(file, "%d\n", getpid());
    fflush(file);

    // genera figli foglie
    pid_t lastForkPid = 1;
    for (int i = 0; i < n && lastForkPid != 0; i++) {
      lastForkPid = fork();
      if (lastForkPid == -1) {
        dprintf(STDERR_FILENO, "Errore fork\n");
        fclose(file);
        exit(GENERIC_FAILURE);
      }
      if (lastForkPid != 0)
        childs[i] = lastForkPid;
    }
    nActiveChilds = n;

    if (lastForkPid == 0) {
      // leaf
      fprintf(file, "%d\n", getpid());
      fclose(file);

      // block signals
      sigset_t mod, old;
      sigemptyset(&mod);
      sigaddset(&mod, SIGCHLD);
      sigaddset(&mod, SIGCONT);
      sigprocmask(SIG_BLOCK, &mod, &old);

      // set sigaction to enable siginfo to recv the pid of sender kill by
      // manager
      struct sigaction sa;
      sigemptyset(&sa.sa_mask);
      sa.sa_sigaction = childRecvdKill;
      sa.sa_flags = SA_SIGINFO;
      sigaction(SIGUSR1, &sa, NULL);

      pause();
      // useless, here just in case
      exit(SUCCESS_EXIT);
    } else if (lastForkPid > 0) {
      // manager
      fclose(file);

      // block signals
      sigset_t mod, old;
      sigemptyset(&mod);
      sigaddset(&mod, SIGALRM);
      sigprocmask(SIG_BLOCK, &mod, &old);

      // set sigaction to enable siginfo to recv pid of sender kill
      struct sigaction sa;
      sigemptyset(&sa.sa_mask);
      sa.sa_sigaction = killChild;
      sa.sa_flags = SA_SIGINFO;
      sigaction(SIGUSR1, &sa, NULL);

      // set sigaction to enable siginfo to recv pid of sender kill
      sigemptyset(&sa.sa_mask);
      sa.sa_sigaction = killManager;
      sa.sa_flags = SA_SIGINFO;
      sigaction(SIGTERM, &sa, NULL);

      while (nActiveChilds > 0)
        pause();

      exit(SUCCESS_EXIT);
    } else {
      dprintf(STDERR_FILENO, "Errore fork\n");
      fclose(file);
      exit(GENERIC_FAILURE);
    }
  } else if (child_pid < 0) {
    dprintf(STDERR_FILENO, "Errore fork\n");
    fclose(file);
    exit(GENERIC_FAILURE);
  }

  fclose(file);
  exit(SUCCESS_EXIT);
}
