#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* Eserczio 5 C Lab Slides 5
 * Avendo come argomenti dei “binari”, si eseguono con exec ciascuno in un
 * sottoprocesso
 */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <exec>\n", argv[0]);
    return 1;
  }

  int argvIndex = argc;
  while (--argvIndex > 0) {
    int isChild = !fork();
    if (isChild)
      execv(argv[argvIndex], NULL);
  }

  while (wait(NULL) > 0)
    ;

  return 0;
}
