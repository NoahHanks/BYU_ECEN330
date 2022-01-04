#include "globals.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1

uint8_t fullSequence[GLOBALS_MAX_FLASH_SEQUENCE] = {ZERO_INIT};
uint16_t newlength = ZERO_INIT;
uint16_t level;

// This is the length of the complete sequence at maximum length.
// You must copy the contents of the sequence[] array into the global variable
// that you maintain. Do not just grab the pointer as this will fail.
void globals_setSequence(const uint8_t sequence[], uint16_t length) {
  for (int i = ZERO_INIT; i <= length; i++) {
    fullSequence[i] = sequence[i];
  }
  newlength = length;
}

// This returns the value of the sequence at the index.
uint8_t globals_getSequenceValue(uint16_t index) { return fullSequence[index]; }

// Retrieve the sequence length.
uint16_t globals_getSequenceLength() { return newlength; }

// This is the length of the sequence that you are currently working on.
void globals_setSequenceIterationLength(uint16_t length) {
  level = length - ONE_INIT;
}

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the person playing the game works through the pattern one color at a time.
uint16_t globals_getSequenceIterationLength() { return level; }