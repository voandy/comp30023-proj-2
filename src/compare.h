#ifndef COMPARE_H
#define COMPARE_H

#include "sha256.h"

#define NUM_FREQ_CHARS 65

typedef long long INTEGER;

typedef struct {
  char c;
  double freq;
} CHAR_FREQ;

void crack(BYTE ** passwords, int passwordCount);
void generate(INTEGER n);
int dictAttack(BYTE ** passwords, int passwordCount, char * dictfile,
  INTEGER generate);

#endif
