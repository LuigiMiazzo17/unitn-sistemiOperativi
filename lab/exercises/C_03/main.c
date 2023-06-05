#include <stdio.h>
#include <stdlib.h>

/* Esercizio 3 C Lab Slides 3
 * Realizzare funzioni per stringhe char *stringrev(char * str) (inverte ordine
 * caratteri) e int stringpos(char * str, char chr) (cerca chr in str e
 * restituisce la posizione)
 */

char *stringrev(const char *str) {
  int len = 0;
  while (str[len++] != '\0')
    ;
  len--;
  char *rev = malloc(len);
  int i;
  for (i = 0; i < len; i++)
    rev[i] = str[len - i - 1];
  return rev;
}

int stringpos(const char *str, const char chr) {
  for (int i = 0; str[i] != '\0'; i++)
    if (str[i] == chr)
      return i;

  return -1;
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <string>\n", argv[0]);
    return 1;
  }
  char *rev = stringrev(argv[1]);
  printf("Reversed input: %s\n", rev);
  free(rev);
  int pos = stringpos(argv[1], 'a');
  printf("Position of 'a': %d\n", pos);
  return 0;
}
