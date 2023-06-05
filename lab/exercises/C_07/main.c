#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define TMPFILE ".tmp"

/*  Esercizio 7 C Lab Slides 5
 *  Dati due eseguibili come argomenti del tipo ls e wc si eseguono in due
 *  processi distinti: il primo deve generare uno stdout redirezionato su un
 * file temporaneo, mentre il secondo deve essere lanciato solo quando il primo
 * ha finito leggendo lo stesso file come stdin. Ad esempio ./main ls wc deve
 * avere lo stesso effetto di ls | wc.
 */

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <exec> | <exec>\n", argv[0]);
    return 1;
  }

  int tmpfile = open(TMPFILE, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
  if (tmpfile == -1) {
    printf("Error opening file!\n");
    return 1;
  }

  int isChild = !fork();
  if (isChild) {
    dup2(tmpfile, STDOUT_FILENO);
    dup2(tmpfile, STDERR_FILENO);
    execlp(argv[1], argv[1], NULL);
    printf("Error executing %s!\n", argv[1]);
    return 1;
  }
  int status;
  wait(&status);
  dup2(tmpfile, STDIN_FILENO);
  lseek(STDIN_FILENO, 0, SEEK_SET);
  isChild = !fork();
  if (isChild) {
    execlp(argv[2], argv[2], NULL);
    printf("Error executing %s!\n", argv[2]);
    return 1;
  }

  while (wait(NULL) > 0)
    ;

  close(tmpfile);
  remove(TMPFILE);
  return 0;
}
