#include "clockControl.h"
#include "clockDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define INIT_ST_MSG "init state\n"
#define NEVER_TOUCHED_ST_MSG "never_touched_st\n"
#define WAITING_FOR_TOUCH_ST_MSG "waiting for touch_st\n"
#define AD_TIMER_RUNNING_ST_MSG "ad_timer_running_st\n"
#define AUTO_TIMER_RUNNING_ST_MSG "auto_timer_running_st\n"
#define RATE_TIMER_RUNNING_ST_MSG "rate_timer_running_st\n"
#define RATE_TIMER_EXPIRED_ST_MSG "rate_timer_expired_st\n"
#define DEFAULT_ERROR_MESSAGE "encountered unknown state error\n"

#define ADD_FINISHED 1
#define AUTO_FINISHED 5
#define RATE_FINISHED 1
#define TOUCH_FINISHED 10

// This contains all the states possible.
enum clockControl_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  never_touched_st, // Wait here until the first touch - clock is disabled until
                    // set.
  waiting_for_touch_st,  // waiting for touch, clock is enabled and running.
  ad_timer_running_st,   // waiting for the touch-controller ADC to settle.
  auto_timer_running_st, // waiting for the auto-update delay to expire
                         // (user is holding down button for auto-inc/dec)
  rate_timer_running_st, // waiting for the rate-timer to expire to know when to
                         // perform the auto inc/dec.
  rate_timer_expired_st, // when the rate-timer expires, perform the inc/dec
                         // function.
  add_second_to_clock_st // add a second to the clock time and reset the ms
                         // counter.
} currentState;

int8_t addTimer = 0;
int16_t autoTimer = 0;
int16_t rateTimer = 0;
int8_t displayTouched = 0;
int8_t countersAdded = 0;

// Checks the current state to see which state we are in and outputs it to the
// console.
void debugStatePrint() {
  static enum clockControl_st_t previousState;
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
    case never_touched_st:
      printf(NEVER_TOUCHED_ST_MSG);
      break;
    case waiting_for_touch_st:
      printf(WAITING_FOR_TOUCH_ST_MSG);
      break;
    case ad_timer_running_st:
      printf(AD_TIMER_RUNNING_ST_MSG);
      break;
    case auto_timer_running_st:
      printf(AUTO_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_running_st:
      printf(RATE_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_expired_st:
      printf(RATE_TIMER_EXPIRED_ST_MSG);
      break;
    }
  }
}

void clockControl_init() { debugStatePrint(); }

// Causes the state machine to tick once per period.
void clockControl_tick() {
  clockControl_init();
  debugStatePrint();
  // Checks the current state then performs the outputs in that case.
  switch (currentState) {
  // Doesn't do anything.
  case init_st:
    break;
  // Doesn't do anything.
  case never_touched_st:
    break;
  // Runs clock at normal speed until display is touched.
  case waiting_for_touch_st:
    addTimer = 0;
    autoTimer = 0;
    rateTimer = 0;
    // If the display has been touched.
    if (displayTouched) {
      // Checks if enough time has passed to see if the touch on the display is
      // done.
      if (countersAdded == TOUCH_FINISHED) {
        clockDisplay_advanceTimeOneSecond();
        clockDisplay_updateTimeDisplay(0);
        countersAdded = 0;
      } else {
        countersAdded++;
      }
    }
    break;

  // Waits for the signal from the diaply to stop to start the timer.
  case ad_timer_running_st:
    // Checks if the displayed is not touched any more to make the clock normal
    // again.
    if (!displayTouched) {
      displayTouched = 1;
    }
    addTimer++;
    break;
  // Waits before running the auto clock quickly.
  case auto_timer_running_st:
    autoTimer++;
    break;
  // Makes the clcok run fast while being held.
  case rate_timer_running_st:
    rateTimer++;
    break;
  // Resets the clock after not deing held any more.
  case rate_timer_expired_st:
    rateTimer = 0;
    break;
  }

  // Switches between the various states.
  switch (currentState) {
  // Goes to the next state.
  case init_st:
    currentState = waiting_for_touch_st;
    break;
  // Waits for touch on the display.
  case waiting_for_touch_st:
    // Clears data and progresses state if touched.
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = ad_timer_running_st;
    }
    break;

  // Goes to the net state when the display is no longer held.
  case ad_timer_running_st:
    // Checks if the audio digital signal is stable.
    if (display_isTouched() && addTimer == ADD_FINISHED) {
      currentState = auto_timer_running_st;
    } else if (!display_isTouched() && addTimer == ADD_FINISHED) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  // Checks the display being held.
  case auto_timer_running_st:
    // Checks the display is touched and held.
    if (display_isTouched() && autoTimer == AUTO_FINISHED) {
      currentState = rate_timer_running_st;
    } else if (!display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  // Goes back and forth to the next state when quickly incrementing or
  // decrementing.
  case rate_timer_running_st:
    // If the display is held and the rate is done.
    if (display_isTouched() && rateTimer == RATE_FINISHED) {
      currentState = rate_timer_expired_st;
    } else if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;
  // Changes the time after progressing from the previous state.
  case rate_timer_expired_st:
    // If display is held.
    if (display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    } else if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;
  default:
    printf(DEFAULT_ERROR_MESSAGE);
    break;
  }
}
