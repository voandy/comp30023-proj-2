#ifndef COMPARE_H
#define COMPARE_H

#include "sha256.h"

typedef long long INTEGER;

void crack(BYTE ** passwords, int passwordCount);
void generate(INTEGER n);

#endif
