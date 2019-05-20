#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <stdbool.h>

#include "sha256.h"

typedef enum
{
    UNLIMITED,
    LIMITED,
    MATCH
} MODE;

#define PWD4 "pwd4sha256"
#define PWD6 "pwd6sha256"

int countPasswords(char *filename);
BYTE ** readPasswords(int passwordCount, char * filename);

int main(int argc, char *argv[]) {

  MODE mode;

  switch (argc) {
    case 1:
    mode = UNLIMITED;
    break;
    case 2:
    mode = LIMITED;
    break;
    case 3:
    mode = MATCH;
    break;
  }

  switch (mode) {
    case UNLIMITED:
    break;
    case LIMITED:
    break;
    case MATCH:
    break;
  }

  BYTE ** passwords;
  int passwordCount = countPasswords("pwd4sha256");

  passwords = readPasswords(passwordCount, "pwd4sha256");

  // for (int j=0; j<passwordCount; j++){
  //   for (int i=0; i<SHA256_BLOCK_SIZE; i++) {
  //     printf("0X%02x ", passwords[j][i]);
  //   }
  //   printf("\n");
  // }


  for (int i=0; i<passwordCount; i++) {
    free(passwords[i]);
  }
  free(passwords);

  return 0;
}

// given a filename, opens the file and counts the number of hashes
int countPasswords(char *filename) {
  FILE *fp;
  int passwordCount;

  fp = fopen(filename, "r");

  if (fp == NULL) {
    perror("Unable to open file.\n");
    exit(EXIT_FAILURE);
  }

  // calculate file size
  fseek(fp, 0L, SEEK_END);
  int fileSize = ftell(fp);

  // does not contain valid number of hashes
  if (fileSize % SHA256_BLOCK_SIZE != 0) {
    perror("Invalid file.\n");
    exit(EXIT_FAILURE);
  }

  passwordCount = fileSize / SHA256_BLOCK_SIZE;

  fclose(fp);

  return passwordCount;
}

// reads password hashes from file into array
BYTE ** readPasswords(int passwordCount, char * filename) {
  BYTE ** passwords;

  passwords = malloc(passwordCount * sizeof(BYTE *));
  for (int i=0; i<passwordCount; i++) {
    passwords[i] = malloc(SHA256_BLOCK_SIZE * sizeof(BYTE));
  }

  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL) {
    perror("Unable to open file.\n");
    exit(EXIT_FAILURE);
  }

  for (int i=0; i<passwordCount; i++) {
    for (int j=0; j<SHA256_BLOCK_SIZE; j++) {
      passwords[i][j] = fgetc(fp);
    }
  }

  return passwords;
}
