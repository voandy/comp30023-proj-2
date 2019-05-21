#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "compare.h"
#include "generate.h"

typedef enum {
    CRACK,
    GUESSES,
    MATCH
} MODE;

#define PWDS "test/pwdsha256"

static int countPasswords(char *filename);
static BYTE ** readPasswords(int passwordCount, char * filename);

int main(int argc, char *argv[]) {
  MODE mode;

  switch (argc) {
    case 1:
    mode = CRACK;
    break;
    case 2:
    mode = GUESSES;
    break;
    case 3:
    mode = MATCH;
    break;
    default:
    perror("Too many arguments.\n");
    exit(EXIT_FAILURE);
  }

  if (mode == CRACK) {
    BYTE ** passwords;
    int passwordCount;

    passwordCount = countPasswords(PWDS);
    passwords = readPasswords(passwordCount, PWDS);

    crack(passwords, passwordCount);

    for (int i=0; i<passwordCount; i++) {
      free(passwords[i]);
    }
    free(passwords);

    return 0;
  }

  if (mode == GUESSES) {
    INTEGER numGuesses = atoll(argv[1]);
    return 0;
  }

  if (mode == MATCH) {
    // char * guesses = argv[1];
    // char * passwords = argv[2];
    return 0;
  }

  return 0;
}

// given a filename, opens the file and counts the number of hashes
static int countPasswords(char *filename) {
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
static BYTE ** readPasswords(int passwordCount, char * filename) {
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
