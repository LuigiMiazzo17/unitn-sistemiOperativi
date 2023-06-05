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

  int infile = open(argv[1], O_RDONLY);
  if (infile == -1) {
    printf("File not found or cannot be opened.\n");
    return 1;
  }

  FILE *outfile;
  char outFilename[256];
  strcpy(outFilename, argv[1]);
  strcat(outFilename, ".copy");
  outfile = fopen(outFilename, "w+");
  if (outfile == NULL) {
    printf("Error opening write file.\n");
    close(infile);
    return 1;
  }

  char buff[1024];
  int r = read(infile, buff, sizeof(buff) - 1);

  while (r != 0) {
    fputs(buff, outfile);
    r = read(infile, buff, sizeof(buff) - 1);
  }

  close(infile);
  fclose(outfile);
  return 0;
}
