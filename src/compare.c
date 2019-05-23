#include "compare.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define RAND_LIM 1000000

#define DICT_4 "refs/common_4_pwds.txt"
#define DICT_6 "refs/common_6_pwds.txt"

#define FREQ_CHARS "aeoilnrsmtchuypkdb2vgf1jw"
#define ALL_CHARS " !\"#$%&'()*+,-./0123456789:;<=>?`@ABCDEFGHIJKLMNOPQRSTUVWXY\
Z[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

static int comparePWD(BYTE guess[], BYTE ** passwords, int passwordCount,
  BYTE buf[SHA256_BLOCK_SIZE], SHA256_CTX ctx);
static int randomGuess(INTEGER generate);
static char randChar(CHAR_FREQ * charFreqs);
static int stringPerms(BYTE ** passwords, int passwordCount, char * characters,
  INTEGER generate);

// attempt to crack given passwords using a variety of guess methods
void crack(BYTE ** passwords, int passwordCount) {
  // try most common passwords first
  dictAttack(passwords, passwordCount, DICT_6, 0);

  // try all permutations of frequently used characters
  stringPerms(passwords, passwordCount, FREQ_CHARS, 0);

  // try all possible permutations
  stringPerms(passwords, passwordCount, ALL_CHARS, 0);
}

// generate and print guesses
void generate(INTEGER n) {
  // dummy pointer, unused
  BYTE ** passwords;
  memset(&passwords, 0, sizeof(passwords));

  // first print known and common passwords
  n -= dictAttack(passwords, 0, DICT_6, n);
  if (n <= 0)
    return;

  // then print some random guesses using common characters
  n-= randomGuess(n);
  if (n <= 0)
    return;

  // try all permutations of frequently used characters
  n -= stringPerms(passwords, 0, FREQ_CHARS, n);
  if (n <= 0)
    return;

  // try all possible permutations
  n -= stringPerms(passwords, 0, ALL_CHARS, n);
  if (n <= 0)
    return;

  perror("All possible combinations generated");
  return;
}

// hash and guess all the words in a given file
int dictAttack(BYTE ** passwords, int passwordCount, char * dictfile,
  INTEGER generate) {

  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;
  memset(&ctx, 0, sizeof(ctx));

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
  }

  fclose(dict);

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

// tries random passwords based on most frequent characters
static int randomGuess(INTEGER generate) {
  CHAR_FREQ charFreqs[NUM_FREQ_CHARS];

  char * line = NULL;
  size_t len = 0;

  FILE *freqFile = fopen ("refs/freqs.csv", "r");

  for (int i=0; i<NUM_FREQ_CHARS; i++) {
    getline(&line, &len, freqFile);
    charFreqs[i].c =line[0];
    char * freq = &line[2];
    charFreqs[i].freq = atof(freq);
  }

  INTEGER count = 0;

  char guess[7];

  while (1) {
    for (int i=0; i<6; i++) {
      guess[i] = randChar(charFreqs);
    }
    guess[6] = 0;
    printf("%s\n", guess);

    count++;
    if (count >= generate || count >= RAND_LIM) {
      return count;
    }
  }
  return count;
}

// returns a random character based on frequencies given
static char randChar(CHAR_FREQ * charFreqs) {
  char c = ' ';
  double randomNum = (double)rand() / (double)RAND_MAX;

  for(int i=0; i<NUM_FREQ_CHARS; i++) {
    if(randomNum <= charFreqs[i].freq) {
      c = charFreqs[i].c;
      break;
    }
  }

  return c;
}

// generates and tests all 6 char perumations given a set of characters
static int stringPerms(BYTE ** passwords, int passwordCount, char * characters,
  INTEGER generate) {

  BYTE buf[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;
  memset(&ctx, 0, sizeof(ctx));

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
