#include "compare.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define DICT_4 "refs/common_4_pwds.txt"
#define DICT_6 "refs/common_6_pwds.txt"

#define FOUND "found_pwds.txt"

void dictAttack(BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx, char * dictfile);

void crack(BYTE ** passwords, int passwordCount) {
  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;

  dictAttack(passwords, passwordCount, buf, ctx, DICT_4);
  dictAttack(passwords, passwordCount, buf, ctx, DICT_6);

}

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
    line[characters - 1] = 0;
    comparePWD((BYTE *)line, passwords, passwordCount, buf, ctx);
  }

  fclose(dict);

  return;
}

// hashes a guess and compares against all given hashes
int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx) {

  sha256_init(&ctx);
  sha256_update(&ctx, guess, strlen((const char*)guess));
  sha256_final(&ctx, buf);

  for (int i=0; i<passwordCount; i++){
    if (!memcmp(passwords[i], buf, SHA256_BLOCK_SIZE)) {
      printf("%s %d\n", guess, i + 1);
      return i;
    }
  }

  return 0;
}
