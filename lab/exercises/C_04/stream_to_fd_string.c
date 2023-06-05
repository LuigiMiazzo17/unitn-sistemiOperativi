#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Esercizio 4 C Lab Slides 4
 * Crea un’applicazione che copia il contenuto di un file, leggendolo con i file
 * streams e scrivendolo con i file descriptors. Crea poi un programma che fa il
 * contrario. Prova a copiare carattere per carattere e linea per linea.
 */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  FILE *infile;
  infile = fopen(argv[1], "r");
  if (infile == NULL) {
    printf("File not found or cannot be opened.\n");
    return 1;
  }
  char outFilename[256];
  strcpy(outFilename, argv[1]);
  strcat(outFilename, ".copy");
  int outfile = open(outFilename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (outfile == -1) {
    printf("Error opening write file.\n");
    fclose(infile);
    return 1;
  }

  char buff[256];
  fgets(buff, sizeof(buff), infile);

  while (!feof(infile)) {
    write(outfile, buff, strlen(buff));
    fgets(buff, sizeof(buff), infile);
  }

  fclose(infile);
  close(outfile);
  return 0;
}
