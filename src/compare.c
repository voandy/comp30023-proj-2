#include "compare.h"

#include <stdio.h>
#include <memory.h>

void crack(BYTE ** passwords, int passwordCount) {
  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;

  BYTE guess[] = {"truelo"};

  comparePWD(guess, passwords, passwordCount, buf, ctx);
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
