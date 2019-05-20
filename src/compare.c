#include "compare.h"

#define _GNU_SOURCE
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

void dictAttack(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * dictfile);
void stringPerms(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * characters);

void crack(BYTE ** passwords, int passwordCount) {
  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;

  // try most common passwords first
  dictAttack(passwords, passwordCount, buf, ctx, DICT_6);

  // no dict match, try all combination of small letters
  // stringPerms(passwords, passwordCount, buf, ctx, ALPHA);

  // still no luck, try all possible combinations
  char chars[CHARS] = ALPHA;
  strcat(chars, CAPS);
  strcat(chars, NUM);
  strcat(chars, SPEC);
  printf("%s\n", chars);
  stringPerms(passwords, passwordCount, buf, ctx, chars);

}

// hash and guess all the words in a given dict file
void dictAttack(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * dictfile) {

  FILE *dict = fopen (dictfile, "r");

  if (dict == NULL) {
    perror (dictfile);
    return;
  }

  char * line = NULL;
  size_t len = 0;
  size_t characters;

  while ((characters = getline(&line, &len, dict)) != -1) {
    // replace the new line char
    line[characters - 1] = 0;
    comparePWD((BYTE *)line, passwords, passwordCount, buf, ctx);
  }

  fclose(dict);

  return;
}

// generates and tests all 6 char perumations given a set of characters
void stringPerms(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * characters) {
  int size = strlen(characters);
  BYTE guess[6];

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

              comparePWD(guess, passwords, passwordCount, buf, ctx);
            }
          }
        }
      }
    }
  }
}

// hashes a guess and compares against all given hashes
int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
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
