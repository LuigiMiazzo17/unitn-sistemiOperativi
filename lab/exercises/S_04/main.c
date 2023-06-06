// Luigi Miazzo 2023-06-06

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

FILE *logfile;

struct msgbuf {
  long mtype;
  pid_t mtext;
};

void handleError(const char *msg) {
  dprintf(STDERR_FILENO, "%s\n", msg);
  if (logfile != NULL)
    fclose(logfile);
  exit(EXIT_FAILURE);
}

void signalRepeater(int sig, siginfo_t *info, void *context) {
  if (sig != SIGUSR1)
    handleError("Something bad happended\n");

  pid_t sender_pid = info->si_pid;
  if (sender_pid < 0)
    handleError("Something bad happended\n");

  kill(sender_pid, SIGUSR1);

  pid_t child_pid = fork();
  if (child_pid == 0) {
    sleep(3);
    fprintf(logfile, "%d-%d\n", sender_pid, sig);
    fclose(logfile);
    exit(EXIT_SUCCESS);
  }
}

void signalRelay(int sig, siginfo_t *info, void *context) {
  if (sig != SIGUSR2)
    handleError("Something bad happended\n");

  pid_t sender_pid = info->si_pid;
  if (sender_pid < 0)
    handleError("Something bad happended\n");

  pid_t child_pid = fork();
  if (child_pid == 0) {
    // child
    kill(sender_pid, SIGUSR2);
    sleep(3);
    fprintf(logfile, "%d-%d\n", sender_pid, sig);
    fclose(logfile);
    exit(EXIT_SUCCESS);
  }
}

void signalInterrupt(int sig) {
  fprintf(logfile, "stop\n");
  fclose(logfile);
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {

  if (argc != 2) {
    dprintf(STDERR_FILENO, "Wrong usage: %s <pathToLogFile\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  logfile = fopen(argv[1], "a");
  if (logfile == NULL)
    handleError("Impossibilie accedere al log file\n");

  if (signal(SIGINT, signalInterrupt) == SIG_ERR)
    handleError("Erorr setting singal handler\n");

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = signalRepeater;
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGUSR1, &sa, NULL) == -1)
    handleError("Erorr setting singal handler\n");

  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = signalRelay;
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGUSR2, &sa, NULL) == -1)
    handleError("Erorr setting singal handler\n");

  key_t queueKey = ftok(argv[1], 1);
  if (queueKey == -1)
    handleError("Error on get queue key\n");

  int queueId = msgget(queueKey, IPC_CREAT | 0777);
  if (queueId == -1)
    handleError("Error on get queue id\n");

  struct msgbuf buff;
  while (1) {
    msgrcv(queueId, &buff, sizeof(buff.mtext), 0, 0);
    kill(buff.mtext, SIGALRM);
  }
}
