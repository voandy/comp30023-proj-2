#include "compare.h"

// #define _GNU_SOURCE
// #define _POSIX_C_SOURCE=200809L
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define DICT_4 "refs/common_4_pwds.txt"
#define DICT_6 "refs/common_6_pwds.txt"

#define FOUND "found_pwds.txt"

#define ALPHA "aeoirsntlmdcbkuhgpyfjvwzxq"
#define CAPS "AESRTBMDNCLOGIHKPFJUYVWZXQ"
#define NUM "1203549678"
#define SPEC "-._*!'@$?#;%&^+~=[\\`]|{}"

#define CHARS 95

static int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx);
static int dictAttack(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * dictfile,
  INTEGER generate);
static int stringPerms(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * characters,
  INTEGER generate);

void crack(BYTE ** passwords, int passwordCount) {
  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;
  memset(&ctx, 0, sizeof(ctx));

  // try most common passwords first
  dictAttack(passwords, passwordCount, buf, ctx, DICT_6, 0);

  // try all combination of small letters
  stringPerms(passwords, passwordCount, buf, ctx, ALPHA, 0);

  // try all possible combinations
  char chars[CHARS] = ALPHA;
  strcat(chars, CAPS);
  strcat(chars, NUM);
  strcat(chars, SPEC);
  printf("%s\n", chars);
  stringPerms(passwords, passwordCount, buf, ctx, chars, 0);
}

// hash and guess all the words in a given file
static int dictAttack(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * dictfile,
  INTEGER generate) {

  INTEGER count = 0;
  FILE *dict = fopen (dictfile, "r");

  if (dict == NULL) {
    perror (dictfile);
    return 0;
  }

  char * line = NULL;
  size_t len = 0;
  size_t characters;

  while ((characters = getline(&line, &len, dict)) != -1) {
    // replace the new line char
    line[characters - 1] = 0;
    if (generate) {
      printf("%s\n", line);
      count++;
      if (count >= generate) {
        return count;
      }
    } else {
      comparePWD((BYTE *)line, passwords, passwordCount, buf, ctx);
    }
    count++;
  }

  fclose(dict);

  return count;
}

// generates and tests all 6 char perumations given a set of characters
static int stringPerms(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * characters,
  INTEGER generate) {

  INTEGER count = 0;
  int size = strlen(characters);
  BYTE guess[7];

  // forgive me Dennis Ritchie for I have sinned
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      for (int k=0; k<size; k++) {
        for (int l=0; l<size; l++) {
          for (int m=0; m<size; m++) {
            for (int n=0; n<size; n++) {
              guess[0] = characters[i];
              guess[1] = characters[j];
              guess[2] = characters[k];
              guess[3] = characters[l];
              guess[4] = characters[m];
              guess[5] = characters[n];
              guess[6] = 0;

              if (generate) {
                printf("%s\n", guess);
                count++;
                if (count >= generate) {
                  return count;
                }
              } else {
                comparePWD(guess, passwords, passwordCount, buf, ctx);
              }
            }
          }
        }
      }
    }
  }
  return count;
}

// hashes a guess and compares against all given hashes
static int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx) {

  sha256_init(&ctx);
  sha256_update(&ctx, guess, strlen((const char*)guess));
  sha256_final(&ctx, buf);

  for (int i=0; i<passwordCount; i++) {
    if (!memcmp(passwords[i], buf, SHA256_BLOCK_SIZE)) {
      printf("%s %d\n", guess, i + 1);
      return i;
    }
  }

  return 0;
}
