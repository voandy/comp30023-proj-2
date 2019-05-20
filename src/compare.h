#ifndef COMPARE_H
#define COMPARE_H

#include "sha256.h"

void crack(BYTE ** passwords, int passwordCount);
int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx);

#endif
