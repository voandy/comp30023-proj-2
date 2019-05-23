#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "compare.h"

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

  // 0 arg mode: crack passwords
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

  // 1 arg mode: generate and print guesses
  if (mode == GUESSES) {
    INTEGER n = atoll(argv[1]);

    /*
    This is a multi tiered attack. First a dictionary attack is performed
    with all the 6 character passwords in the provided common_passwords.txt
    appended with other student's found passwords.
    The we try some randomly generated guesses based on frequency analysis
    of the character in common_passwords.txt.
    Then we try all possible permutation of passwords using the 25 most frequent
    characters.
    Then to find the remaining high entropy passwords we try all possible
    permutations within the full character set.
    */
    generate(n);
    return 0;
  }

  // 2 arg mode: match passwords with hashes
  if (mode == MATCH) {
    char * guessFile = argv[1];
    char * pwdsFile = argv[2];

    BYTE ** passwords;
    int passwordCount;

    passwordCount = countPasswords(pwdsFile);
    passwords = readPasswords(passwordCount, pwdsFile);

    dictAttack(passwords, passwordCount, guessFile, 0);

    for (int i=0; i<passwordCount; i++) {
      free(passwords[i]);
    }
    free(passwords);

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
