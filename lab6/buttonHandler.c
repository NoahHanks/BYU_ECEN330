#include "buttonHandler.h"
#include "display.h"
#include "simonDisplay.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1
#define ADC_SETTLED 1
#define NOT_A_REGION 5

#define INIT_ST_MSG "bh_init_st\n"
#define DRAW_BUTTONS_ST_MSG "bh_draw_buttons_st\n"
#define WAIT_FOR_TOUCH_ST_MSG "bh_wait_for_touch_st\n"
#define SETTLE_ST_MSG "bh_settle_st\n"
#define IS_TOUCHED_ST_MSG "bh_is_touched_st\n"
#define UNTOUCHED_ST_MSG "bh_untouched_st\n"
#define END_ST_MSG "bh_end_st\n"

uint8_t enableBH = ZERO_INIT;
uint8_t ADCsettleCounter = ZERO_INIT;
bool buttonsDrawn = false;
bool screenReleased = false;
uint8_t region = NOT_A_REGION;
bool timeOutOccurred = false;
bool BHComplete = false;

// The buttonHandler states.
enum buttonHandler_st_t {
  init_st,
  draw_buttons_st,
  wait_for_touch_st,
  settle_st,
  is_touched_st,
  untouched_st,
  end_st
} currentState = init_st;

void debugStatePrint() {
  static enum buttonHandler_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case draw_buttons_st:
      printf(DRAW_BUTTONS_ST_MSG);
      break;
    case wait_for_touch_st:
      printf(WAIT_FOR_TOUCH_ST_MSG);
      break;
    case settle_st:
      printf(SETTLE_ST_MSG);
      break;
    case is_touched_st:
      printf(IS_TOUCHED_ST_MSG);
      break;
    case untouched_st:
      printf(UNTOUCHED_ST_MSG);
      break;
    case end_st:
      printf(END_ST_MSG);
      break;
    }
  }
}

// Standard init function.
void buttonHandler_init() {}

// Get the simon region numbers. See the source code for the region numbering
// scheme.
// checks x and y input do determine region value.
uint8_t buttonHandler_getRegionNumber() {
  int16_t x, y = ZERO_INIT;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);
  return simonDisplay_computeRegionNumber(x, y);
}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable() { enableBH = ONE_INIT; }

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable() { enableBH = ZERO_INIT; }

/// The only thing this function does is return a boolean flag set by the
// buttonHandler state machine. To wit: Once enabled, the buttonHandler
// state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area.
// Next, the buttonHandler state-machine waits until the player removes their
// finger. At this point, the state-machine should set a bool flag that
// indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
// If this function does more than return a boolean set by the buttonHandler
// state machine, you are going about this incorrectly.
bool buttonHandler_releaseDetected() {
  // checks if the screen was released.
  if (screenReleased) {
    screenReleased = false;
    return true;
  }
  return false;
}

// Let's you know that the buttonHander is waiting in the interlock state.
bool buttonHandler_isComplete() {
  if (BHComplete) {
    return true;
  } else {
    return false;
  }
}

// Allows an external controller to notify the buttonHandler that a time-out has
// occurred.
void buttonHandler_timeOutOccurred() { timeOutOccurred = true; }

// Standard tick function.
void buttonHandler_tick() {
  buttonHandler_init();
  debugStatePrint();
  // Switch statements
  switch (currentState) {
  case init_st:
    break;
  case draw_buttons_st:
    simonDisplay_drawAllButtons();
    break;
  case wait_for_touch_st:
    break;
  case settle_st:
    ADCsettleCounter++;
    break;
  case is_touched_st:
    break;
  case untouched_st:
    break;
  case end_st:
    break;
  }

  // state transitions
  switch (currentState) {
  case init_st:
    // Checks if the buttons need to be drawn or not.
    if (enableBH && buttonsDrawn) {
      currentState = wait_for_touch_st;
    } else if (enableBH) {
      currentState = draw_buttons_st;
    }
    break;
  case draw_buttons_st:
    buttonsDrawn = true;
    currentState = wait_for_touch_st;
    break;
  case wait_for_touch_st:
    // wait for touch.
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = settle_st;
    }
    break;
  case settle_st:
    // waits until the ADC is settled
    if (ADCsettleCounter == ADC_SETTLED) {
      ADCsettleCounter = 0;
      region = buttonHandler_getRegionNumber();
      simonDisplay_drawSquare(region, false);
      currentState = is_touched_st;
    }
    break;
  case is_touched_st:
    // waits until the display isn't pressed
    if (!display_isTouched()) {
      currentState = untouched_st;
    }
    break;
  case untouched_st:
    simonDisplay_drawSquare(region, true);
    simonDisplay_drawButton(region, false);
    screenReleased = true;
    currentState = end_st;
    break;
  case end_st:
    // waits until the buttonHandler isn't enabled.
    if (!enableBH) {
      buttonsDrawn = false;
      BHComplete = true;
      currentState = init_st;
    }
    break;
  }
}

#define RUN_TEST_TEXT_SIZE 2
#define UTILS_DELAY 1

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void buttonHandler_runTest(int16_t touchCountArg) {
  int16_t touchCount = ZERO_INIT;
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  simonDisplay_drawAllButtons();
  buttonHandler_enable();
  // loops until the display has been touched neough times.
  while (touchCount < touchCountArg) {
    buttonHandler_tick();
    utils_msDelay(UTILS_DELAY);
    // checks for a screen release.
    if (buttonHandler_releaseDetected()) {
      touchCount++;
      printf("button released: %d\n\r", buttonHandler_getRegionNumber());
      buttonHandler_disable();
      utils_msDelay(UTILS_DELAY);
      buttonHandler_tick();
      buttonHandler_enable();
      utils_msDelay(UTILS_DELAY);
      buttonHandler_tick();
    }
  }
}
