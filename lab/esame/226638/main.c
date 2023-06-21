#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

int canContinue;

// funzione di servizio per panicare in caso di errore
void handleError(const char*msg){
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);    
}

// struttura per i parametri di killAProcess()
struct kill_s{
    int signo;
    pid_t pid;
};

// funzione che invia il segnale al processo desiderato
void* killAProcess(void *params){
    struct kill_s *args = (struct kill_s *)params;
    if(kill(args->pid, args->signo) == -1)
        fprintf(stderr, "Kill failed\n");
}

// struttura per i parametri di sendQueue()
struct queue_s{
    int queueId;
    int category;
    char *msg;
};

// struttura per il messaggio queue
struct msgbuf {
  long mtype;
  char mtext[256];
};

// funzione che manda il messaggio alla queue desiderata
void* sendQueue(void *params){
    struct queue_s *args = (struct queue_s*)params;

    struct msgbuf queueBuff;
    queueBuff.mtype = args->category;
    strncpy(queueBuff.mtext, args->msg, 256);
    if(msgsnd(args->queueId, &queueBuff, sizeof(queueBuff.mtext), 0) == -1)
        fprintf(stderr, "Queue failed\n");
}

// struttura per i parametri di sendFifo()
struct fifo_s{
    char *fifoName;
    char *msg;
};

// funzione che invia alla fifo un messaggio
void* sendFifo(void *params){
    struct fifo_s *args = (struct fifo_s*)params;

    if(mkfifo(args->fifoName,S_IRUSR|S_IWUSR))
        fprintf(stderr, "Fifo failed\n");
    int fd = open(args->fifoName, O_WRONLY);
    write(fd, args->msg, strlen(args->msg));
    close(fd);
}

void handleSigUsr1(int sig) {
    if(sig == SIGUSR1)
        canContinue = 1;
}

int main(int argc, char **argv){
    
    // controlla argomenti
    if(argc != 2){
        fprintf(stderr, "Uso errato: %s <pathToFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // apre il file dei comandi
    FILE * cmdFile = fopen(argv[1], "r");
    if(cmdFile == NULL)
        handleError("Il file non esiste");

    // apre la queue
    key_t queueKey = ftok(argv[1], 1);
    if (queueKey == -1) 
        handleError("ftok");
    int queueId = msgget(queueKey, IPC_CREAT | 0777);
    if (queueId == -1) 
        handleError("msgget");

    // buffer di lettura
    char cmdName[256];
    char buff1[256];
    char buff2[256];

    canContinue = 0;

    if (signal(SIGUSR1, handleSigUsr1) == SIG_ERR)
        handleError("signal\n");

    while(fscanf(cmdFile, "%s %s %s\n", cmdName, buff1, buff2) != EOF){
        // funzione kill
        if(strcmp(cmdName, "kill") == 0){
            int signo = atoi(buff1);
            pid_t pid = atoi(buff2);

            if(signo < 1 || signo > 32){
                fprintf(stderr, "Valore invalido\n");
                continue;
            }
            if(pid < 0){
                fprintf(stderr, "Valore invalido\n");
                continue;
            }

            struct kill_s params;
            params.signo = signo;
            params.pid = pid;

            pthread_t t_id;
            pthread_create(&t_id, NULL, killAProcess, (void *)&params);
            
            while(canContinue != 1)
                pause();

            canContinue = 0;

        } else if(strcmp(cmdName, "queue") == 0){
            long category = atoi(buff1);
            if(category <= 0){
                fprintf(stderr, "Valore invalido\n");
                continue;
            }

            struct queue_s params;
            params.queueId = queueId;
            params.category = category;
            params.msg = buff2;

            pthread_t t_id;
            pthread_create(&t_id, NULL, sendQueue, (void *)&params);
            
            while(canContinue != 1)
                pause();

            canContinue = 0;

        } else if(strcmp(cmdName, "fifo") == 0){
            struct fifo_s params;
            params.fifoName = buff1;
            params.msg = buff2;

            pthread_t t_id;
            pthread_create(&t_id, NULL, sendFifo, (void *)&params);
            
            while(canContinue != 1)
                pause();

            canContinue = 0;
        }

    }

    return 0;
}