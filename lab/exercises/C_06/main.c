#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/*  Esercizio 6 C Lab Slides 05
 * 1. Avendo come argomenti dei “binari”, si eseguono con exec ciascuno in un
 * sottoprocesso
 * 2. idem punto 1 ma in più salvando i flussi di stdout e stderr in un unico
 * file
 */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <exec>\n", argv[0]);
    return 1;
  }

  int outfile =
      open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  if (outfile == -1) {
    printf("Error opening file!\n");
    return 1;
  }

  int argvIndex = argc;
  while (--argvIndex > 0) {
    int isChild = !fork();
    if (isChild) {
      dup2(outfile, STDOUT_FILENO);
      dup2(outfile, STDERR_FILENO);
      execlp(argv[argvIndex], argv[argvIndex], NULL);
      printf("Error executing %s!\n", argv[argvIndex]);
      return 1;
    }
  }

  while (wait(NULL) > 0)
    ;

  close(outfile);
  return 0;
}
