#include "flashSequence.h"
#include "display.h"
#include "globals.h"
#include "simonDisplay.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1

#define INIT_ST_MSG "fs_init_st\n"
#define DRAW_SQUARES_ST_MSG "fs_draw_squares_st\n"
#define WAIT_FOR_SQUARES_ST_MSG "fs_wait_for_sqaures_st\n"
#define ERASE_SQUARES_ST_MSG "fs_erase_sqaures_st\n"
#define END_ST_MSG "fs_end_st\n"
#define SQUARE_DURATION 5

uint8_t enableFS = ZERO_INIT;
uint32_t waitCounter = ZERO_INIT;
bool FSComplete = false;
uint16_t sequenceCounterFS = ZERO_INIT;

// The buttonHandler states.
enum flashSequence_st_t {
  init_st,
  draw_squares_st,
  wait_for_squares_st,
  erase_squares_st,
  end_st
} currentStateFS = init_st;

void debugStatePrintFS() {
  static enum flashSequence_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentStateFS - this prevents reprinting the same
  // state name over and over.
  if (previousState != currentStateFS || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentStateFS;       // keep track of the last state that you were in.
    switch (currentStateFS) { // This prints messages based upon the state that
                              // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case draw_squares_st:
      printf(DRAW_SQUARES_ST_MSG);
      break;
    case wait_for_squares_st:
      printf(WAIT_FOR_SQUARES_ST_MSG);
      break;
    case erase_squares_st:
      printf(ERASE_SQUARES_ST_MSG);
      break;
    case end_st:
      printf(END_ST_MSG);
      break;
    }
  }
}

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() { enableFS = ONE_INIT; }

// Turns off the state machine. Part of the interlock.
void flashSequence_disable() { enableFS = ZERO_INIT; }

// Standard init function.
void flashSequence_init() {
  sequenceCounterFS = ZERO_INIT;
  FSComplete = ZERO_INIT;
}

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete() { return FSComplete; }

// Standard tick function.
void flashSequence_tick() {
  debugStatePrintFS();
  switch (currentStateFS) {
  // state actions
  case init_st:
    break;
  case draw_squares_st:
    simonDisplay_drawSquare(globals_getSequenceValue(sequenceCounterFS),
                            ZERO_INIT);
    break;
  case wait_for_squares_st:
    waitCounter++;
    break;
  case erase_squares_st:
    simonDisplay_drawSquare(globals_getSequenceValue(sequenceCounterFS),
                            ONE_INIT);
    break;
  case end_st:
    FSComplete = ONE_INIT;
    break;
  }

  // state transitions
  switch (currentStateFS) {
  case init_st:
    // waits here until the flash sequence is enabled.
    if (enableFS) {
      currentStateFS = draw_squares_st;
    }
    break;
  case wait_for_squares_st:
    if (waitCounter > SQUARE_DURATION) {
      waitCounter = ZERO_INIT;
      currentStateFS = erase_squares_st;
    }
    break;
  case draw_squares_st:
    // after the square has been drawn, go wait
    currentStateFS = wait_for_squares_st;
    break;
  case erase_squares_st:
    // checks if the sequence is complete or not
    if (sequenceCounterFS >= globals_getSequenceIterationLength()) {
      currentStateFS = end_st;
    } else {
      sequenceCounterFS++;
      currentStateFS = draw_squares_st;
    }
    break;
  case end_st:
    // resets the flash sequence if it is now disabled.
    if (!enableFS) {
      flashSequence_init();
      currentStateFS = init_st;
    }
    break;
  }
}
