
#include "verifySequence.h"
#include "buttonHandler.h"
#include "display.h"
#include "globals.h"
#include "simonDisplay.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1

#define TIME_OUT_DURATION 50

#define INIT_ST_MSG "vs_init_st\n"
#define ENABLE_BUTTON_HANDLER_MSG "vs_enable_button_handler_st\n"
#define WAITING_MSG "vs_waiting_st\n"
#define VERIFY_MSG "vs_verify_st\n"
#define INCREMENT_MSG "vs_increment_st\n"
#define END_ST_MSG "vs_end_st\n"

uint8_t enableVS = ZERO_INIT;
bool VSComplete = ZERO_INIT;
bool tooSlow = ZERO_INIT;
uint8_t wrongButton = ZERO_INIT;
uint16_t timeOutCounter = ZERO_INIT;
uint8_t sequenceCounterVS = ZERO_INIT;

// the verify sequence state machine states
enum verifySequence_st_t {
  init_st,
  enable_button_handler_st,
  waiting_st,
  verify_st,
  increment_st,
  end_st

} currentStateVS = init_st;

void debugStatePrintVS() {
  static enum verifySequence_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentStateVS - this prevents reprinting the same
  // state name over and over.
  if (previousState != currentStateVS || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentStateVS;       // keep track of the last state that you were in.
    switch (currentStateVS) { // This prints messages based upon the state that
                              // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case enable_button_handler_st:
      printf(ENABLE_BUTTON_HANDLER_MSG);
      break;
    case waiting_st:
      printf(WAITING_MSG);
      break;
    case verify_st:
      printf(VERIFY_MSG);
      break;
    case increment_st:
      printf(INCREMENT_MSG);
      break;
    case end_st:
      printf(END_ST_MSG);
      break;
    }
  }
}

// State machine will run when enabled.
void verifySequence_enable() { enableVS = ONE_INIT; }

// This is part of the interlock. You disable the state-machine and then enable
// it again.
void verifySequence_disable() { enableVS = ZERO_INIT; }

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError() { return tooSlow; }

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError() { return wrongButton; }

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete() { return VSComplete; }

void verifySequence_init() {
  sequenceCounterVS = ZERO_INIT;
  VSComplete = ZERO_INIT;
  tooSlow = ZERO_INIT;
  wrongButton = ZERO_INIT;
  timeOutCounter = ZERO_INIT;
}

// Standard tick function.
void verifySequence_tick() {
  // state actions
  debugStatePrintVS();
  switch (currentStateVS) {
  case init_st:
    break;
  case enable_button_handler_st:
    buttonHandler_enable();
    break;
  case waiting_st:
    timeOutCounter++;
    break;
  case verify_st:
    buttonHandler_disable();
    // sets wrongButton to true if the button pressed was wrong.
    if (globals_getSequenceValue(sequenceCounterVS) !=
        buttonHandler_getRegionNumber()) {
      wrongButton = true;
    }
    break;
  case increment_st:
    break;
  case end_st:
    break;
  }

  // state transitions
  switch (currentStateVS) {
  case init_st:
    // waits until the VS state machine is enabled
    if (enableVS) {
      verifySequence_init();
      currentStateVS = enable_button_handler_st;
    }
    break;
  case enable_button_handler_st:
    currentStateVS = waiting_st;
    break;
  case waiting_st:
    // checks if the user took too long
    if (timeOutCounter == TIME_OUT_DURATION) {
      simonDisplay_eraseAllButtons();
      VSComplete = ONE_INIT;
      tooSlow = ONE_INIT;
      currentStateVS = end_st;
    } else if (display_isTouched()) {
      timeOutCounter = ZERO_INIT;
    } else if (buttonHandler_releaseDetected()) {
      currentStateVS = verify_st;
    }
    break;
  case verify_st:
    currentStateVS = increment_st;
    break;
  case increment_st:
    // checks if the sequence is done
    if (sequenceCounterVS == globals_getSequenceIterationLength()) {
      simonDisplay_eraseAllButtons();
      VSComplete = true;
      currentStateVS = end_st;
    }
    // checks if the wrong button was pressed
    else if (wrongButton) {
      simonDisplay_eraseAllButtons();
      VSComplete = true;
      currentStateVS = end_st;
    } else {
      sequenceCounterVS++;
      currentStateVS = enable_button_handler_st;
    }
    break;
  case end_st:
    // checks if the VS state machine is disabled.
    if (!enableVS) {
      currentStateVS = init_st;
    }
    break;
  }
}