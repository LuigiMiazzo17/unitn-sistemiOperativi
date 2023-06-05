#include <stdio.h>

/* Esercizio 1 C Lab Slides 3
 * Crea un’applicazione che copia il contenuto di un file, leggendolo con i file
 * streams e scrivendolo con i file descriptors. Crea poi un programma che fa il
 * contrario. Prova a copiare carattere per carattere e linea per linea.
 */

int str_len(const char *str) {
  int len = 0;
  while (str[len++] != '\0')
    ;
  return len - 1;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <string>\n", argv[0]);
    return 1;
  }
  printf("The string \"%s\" is %d characters long.\n", argv[1],
         str_len(argv[1]));
  return 0;
}
