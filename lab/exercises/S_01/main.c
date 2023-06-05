// Luigi Miazzo 2023-06-05

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define SUCCESS 0
#define FAILURE 1

void die(pid_t pid, int status) {
  if (pid > 0)
    kill(pid, status == SUCCESS ? SIGUSR1 : SIGUSR2);
  exit(status);
}

struct msgType {
  long mType;
  char mText[33];
} msgBuff;

int getQueue(char *queueName) {
  if (creat(queueName, 0777) < 0)
    return -1;

  key_t queueKey = ftok(queueName, 1);
  if (queueKey == -1)
    return -1;

  int queueId = msgget(queueKey, IPC_CREAT | 0777);

  if (queueId < 0)
    return -1;

  return queueId;
}

int reciveMsg(int queueId) {
  return msgrcv(queueId, &msgBuff, sizeof(msgBuff.mText), 1, IPC_NOWAIT);
}

int sendMsg(int queueId) {
  return msgsnd(queueId, &msgBuff, sizeof(msgBuff.mText), 0);
}

int main(int argc, char **argv) {

  if (argc != 4 && argc != 5) {
    printf("Usage: %s <name> <action> [<value>] <pid>\n", argv[0]);
    exit(1);
  }

  char *queueName = argv[1];
  char *cmd = argv[2];
  pid_t signalPid = 0;

  if (strcmp(cmd, "new") == 0 || strcmp(cmd, "get") == 0 ||
      strcmp(cmd, "del") == 0 || strcmp(cmd, "emp") == 0)
    signalPid = atoi(argv[3]);
  else if (strcmp(cmd, "put") == 0 || strcmp(cmd, "mov") == 0)
    signalPid = atoi(argv[4]);
  else
    die(-1, FAILURE);

  int queueId = getQueue(queueName);
  if (queueId == -1)
    die(signalPid, FAILURE);

  if (strcmp(cmd, "new") == 0) {
    // new queue
    printf("%d\n", queueId);

    die(signalPid, SUCCESS);
  } else if (strcmp(cmd, "put") == 0) {
    // put value to queue
    char *value = argv[3];

    msgBuff.mType = 1;
    strncpy(msgBuff.mText, value, 32);
    msgBuff.mText[32] = 0;

    if (sendMsg(queueId) == -1)
      die(signalPid, FAILURE);

    die(signalPid, SUCCESS);
  } else if (strcmp(cmd, "get") == 0) {
    // get value from queue
    if (reciveMsg(queueId) != -1)
      printf("%s\n", msgBuff.mText);

    die(signalPid, SUCCESS);
  } else if (strcmp(cmd, "del") == 0) {
    // deletes queue
    if (msgctl(queueId, IPC_RMID, NULL) == -1)
      die(signalPid, FAILURE);

    die(signalPid, SUCCESS);
  } else if (strcmp(cmd, "emp") == 0) {
    // empty the queue
    while (reciveMsg(queueId) != -1)
      printf("%s\n", msgBuff.mText);

    die(signalPid, SUCCESS);
  } else if (strcmp(cmd, "mov") == 0) {
    // mov the queue
    char *destQueue = argv[3];

    int destQueueId = getQueue(destQueue);

    int counter = 0;
    while (reciveMsg(queueId) != -1) {
      printf("%s\n", msgBuff.mText);
      counter++;

      if (sendMsg(destQueueId) == -1)
        die(signalPid, FAILURE);
    }
    printf("%d\n", counter);

    die(signalPid, SUCCESS);
  }

  return 0;
}
