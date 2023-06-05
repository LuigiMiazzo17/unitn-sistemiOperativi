#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_USAGE 10

/* Esercizio 2 C Lab Slides 3
 * Scrivere un’applicazione che definisce una lista di argomenti validi e legge
 * quelli passati alla chiamata verificandoli e memorizzando le opzioni
 * corrette, restituendo un errore in caso di un’opzione non valida.
 */

const char validUsages[MAX_USAGE][32] = {"help",   "version", "list",   "add",
                                         "remove", "edit",    "search", "sort",
                                         "clear",  "exit"};

int checkList(const char *str) {
  for (int i = 0; i < MAX_USAGE; i++)
    if (strcmp(str, validUsages[i]) == 0)
      return i;
  return -1;
}

int main(int argc, char *argv[]) {
  int argc_index = argc;

  bool used[MAX_USAGE];
  for (int i = 0; i < MAX_USAGE; i++)
    used[i] = false;

  while (argc_index > 1) {
    int index = checkList(argv[argc_index - 1]);
    if (-1 == index) {
      printf("Invalid usage: %s\n", argv[argc_index - 1]);
      return 1;
    }
    used[index] = true;
    argc_index--;
  }

  for (int i = 0; i < MAX_USAGE; i++)
    if (used[i])
      printf("Used: %s\n", validUsages[i]);

  return 0;
}
