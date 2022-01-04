#include "clockDisplay.h"
#include "clockControl.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define COLON_TEXT ":"
#define DISPLAY_X_CENTER (DISPLAY_WIDTH / 2)
#define DISPLAY_X_ONE_THIRD (DISPLAY_WIDTH * (1 / 3.0))
#define DISPLAY_X_TWO_THIRDS (DISPLAY_WIDTH * (2 / 3.0))
#define DISPLAY_Y_CENTER (DISPLAY_HEIGHT / 2)

#define LEFT_TEXT_X_CURSOR (DISPLAY_X_CENTER - (22 * CLOCKDISPLAY_TEXT_SIZE))
#define MID_TEXT_X_CURSOR (DISPLAY_X_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define RIGHT_TEXT_X_CURSOR (DISPLAY_X_CENTER + (12.5 * CLOCKDISPLAY_TEXT_SIZE))
#define LEFT_COLON_X_CURSOR (DISPLAY_X_CENTER - (11 * CLOCKDISPLAY_TEXT_SIZE))
#define RIGHT_COLON_X_CURSOR (DISPLAY_X_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TEXT_Y_CURSOR (DISPLAY_Y_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_TOP_LEFT_X0 (DISPLAY_X_CENTER - (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_LEFT_Y0 (DISPLAY_Y_CENTER - (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_LEFT_X1                                                   \
  (TRIANGLE_TOP_MID_X1 - (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_LEFT_Y1 (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_LEFT_X2                                                   \
  (TRIANGLE_TOP_LEFT_X1 + (10 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_LEFT_Y2 (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_TOP_MID_X0 (DISPLAY_X_CENTER)
#define TRIANGLE_TOP_MID_Y0 (DISPLAY_Y_CENTER - (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_MID_X1 (DISPLAY_X_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_MID_Y1 (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_MID_X2 (DISPLAY_X_CENTER + (5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_MID_Y2 (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_TOP_RIGHT_X0 (DISPLAY_X_CENTER + (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_RIGHT_Y0 (DISPLAY_Y_CENTER - (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_RIGHT_X1                                                  \
  (TRIANGLE_TOP_MID_X1 + (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_RIGHT_Y1                                                  \
  (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_RIGHT_X2                                                  \
  (TRIANGLE_TOP_RIGHT_X1 + (10 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_TOP_RIGHT_Y2                                                  \
  (DISPLAY_Y_CENTER - (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_BOT_LEFT_X0 (DISPLAY_X_CENTER - (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_LEFT_Y0 (DISPLAY_Y_CENTER + (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_LEFT_X1                                                   \
  (TRIANGLE_TOP_MID_X1 - (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_LEFT_Y1 (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_LEFT_X2                                                   \
  (TRIANGLE_TOP_LEFT_X1 + (10 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_LEFT_Y2 (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_BOT_MID_X0 (DISPLAY_X_CENTER)
#define TRIANGLE_BOT_MID_Y0 (DISPLAY_Y_CENTER + (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_MID_X1 (DISPLAY_X_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_MID_Y1 (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_MID_X2 (DISPLAY_X_CENTER + (5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_MID_Y2 (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define TRIANGLE_BOT_RIGHT_X0 (DISPLAY_X_CENTER + (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_RIGHT_Y0 (DISPLAY_Y_CENTER + (12 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_RIGHT_X1                                                  \
  (TRIANGLE_TOP_MID_X1 + (18 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_RIGHT_Y1                                                  \
  (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_RIGHT_X2                                                  \
  (TRIANGLE_TOP_RIGHT_X1 + (10 * CLOCKDISPLAY_TEXT_SIZE))
#define TRIANGLE_BOT_RIGHT_Y2                                                  \
  (DISPLAY_Y_CENTER + (6.5 * CLOCKDISPLAY_TEXT_SIZE))

#define RECTANGLE_W (11.5 * CLOCKDISPLAY_TEXT_SIZE)
#define RECTANGLE_H (8.5 * CLOCKDISPLAY_TEXT_SIZE)

#define CLOCK_TEST_DELAY 300
#define CLOCK_TEST_TENTH_SECOND 100
#define CLOCK_TEST_INC_DEC 8

#define FORMAT_HOURS "%2hd"
#define FORMAT_MINTUES_AND_SECONDS "%02hd"
#define MAX_HOURS 12
#define MAX_MINUTES_AND_SECONDS 59
#define HOURS_ROLLOVER 13
#define MINUTES_AND_SECONDS_ROLLOVER 60
#define CLOCK_ROLLDOWN 0

uint8_t newHours = 12;
uint8_t newMinutes = 59;
uint8_t newSeconds = 59;
uint8_t oldHours = 1;
uint8_t oldMinutes = 0;
uint8_t oldSeconds = 0;

char charHours[8];
char charMin[8];
char charSec[8];

// Sets the display and draws all the shapes on it. Also sets all the cursors
// and colors.
void clockDisplay_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  display_setTextSize(CLOCKDISPLAY_TEXT_SIZE);
  display_setCursor(LEFT_TEXT_X_CURSOR, TEXT_Y_CURSOR);
  display_setTextColor(DISPLAY_GREEN);
  display_setCursor(LEFT_COLON_X_CURSOR, TEXT_Y_CURSOR);
  display_println(COLON_TEXT);
  display_setCursor(RIGHT_COLON_X_CURSOR, TEXT_Y_CURSOR);
  display_println(COLON_TEXT);
  display_fillTriangle(TRIANGLE_TOP_MID_X0, TRIANGLE_TOP_MID_Y0,
                       TRIANGLE_TOP_MID_X1, TRIANGLE_TOP_MID_Y1,
                       TRIANGLE_TOP_MID_X2, TRIANGLE_TOP_MID_Y2, DISPLAY_GREEN);
  display_fillTriangle(TRIANGLE_TOP_LEFT_X0, TRIANGLE_TOP_LEFT_Y0,
                       TRIANGLE_TOP_LEFT_X1, TRIANGLE_TOP_LEFT_Y1,
                       TRIANGLE_TOP_LEFT_X2, TRIANGLE_TOP_LEFT_Y2,
                       DISPLAY_GREEN);
  display_fillTriangle(TRIANGLE_TOP_RIGHT_X0, TRIANGLE_TOP_RIGHT_Y0,
                       TRIANGLE_TOP_RIGHT_X1, TRIANGLE_TOP_RIGHT_Y1,
                       TRIANGLE_TOP_RIGHT_X2, TRIANGLE_TOP_RIGHT_Y2,
                       DISPLAY_GREEN);
  display_fillTriangle(TRIANGLE_BOT_LEFT_X0, TRIANGLE_BOT_LEFT_Y0,
                       TRIANGLE_BOT_LEFT_X1, TRIANGLE_BOT_LEFT_Y1,
                       TRIANGLE_BOT_LEFT_X2, TRIANGLE_BOT_LEFT_Y2,
                       DISPLAY_GREEN);
  display_fillTriangle(TRIANGLE_BOT_MID_X0, TRIANGLE_BOT_MID_Y0,
                       TRIANGLE_BOT_MID_X1, TRIANGLE_BOT_MID_Y1,
                       TRIANGLE_BOT_MID_X2, TRIANGLE_BOT_MID_Y2, DISPLAY_GREEN);
  display_fillTriangle(TRIANGLE_BOT_RIGHT_X0, TRIANGLE_BOT_RIGHT_Y0,
                       TRIANGLE_BOT_RIGHT_X1, TRIANGLE_BOT_RIGHT_Y1,
                       TRIANGLE_BOT_RIGHT_X2, TRIANGLE_BOT_RIGHT_Y2,
                       DISPLAY_GREEN);
  clockDisplay_updateTimeDisplay(1);
}

// Updates the clock if we tell it to or if the time has changed.
void clockDisplay_updateTimeDisplay(bool updateClock) {
  // If the hours have changed or we told it to update.
  if (newHours != oldHours || updateClock) {
    oldHours = newHours;
    sprintf(charHours, FORMAT_HOURS, newHours);
    display_setCursor(LEFT_TEXT_X_CURSOR, TEXT_Y_CURSOR);
    display_fillRect(LEFT_TEXT_X_CURSOR, TEXT_Y_CURSOR, RECTANGLE_W,
                     RECTANGLE_H, DISPLAY_BLACK);
    display_println(charHours);
    sprintf(charHours, FORMAT_HOURS, newHours);
  }

  // If the minutes have changed or we told it to update.
  if (newMinutes != oldMinutes || updateClock) {
    oldMinutes = newMinutes;
    sprintf(charMin, FORMAT_MINTUES_AND_SECONDS, newMinutes);
    display_setCursor(MID_TEXT_X_CURSOR, TEXT_Y_CURSOR);
    display_fillRect(MID_TEXT_X_CURSOR, TEXT_Y_CURSOR, RECTANGLE_W, RECTANGLE_H,
                     DISPLAY_BLACK);
    display_println(charMin);
  }
  // If the seconds have changed or we told it to update.
  if (newSeconds != oldSeconds || updateClock) {
    oldSeconds = newSeconds;
    sprintf(charSec, FORMAT_MINTUES_AND_SECONDS, newSeconds);
    display_setCursor(RIGHT_TEXT_X_CURSOR, TEXT_Y_CURSOR);
    display_fillRect(RIGHT_TEXT_X_CURSOR, TEXT_Y_CURSOR, RECTANGLE_W,
                     RECTANGLE_H, DISPLAY_BLACK);
    display_println(charSec);
  }
}

// Increments hours and rolls over if needed.
void incHours() {
  newHours += 1;
  // Rolls over if needed.
  if (newHours == HOURS_ROLLOVER) {
    newHours = 1;
  }
}

// Decrements hours and rolls down if needed.
void decHours() {
  newHours -= 1;
  // Rolls down if needed.
  if (newHours == CLOCK_ROLLDOWN) {
    newHours = MAX_HOURS;
  }
}

// Increments minutes and rolls over if needed.
void incMin() {
  newMinutes += 1;
  // Rolls over if needed.
  if (newMinutes == MINUTES_AND_SECONDS_ROLLOVER) {
    newMinutes = 0;
  }
}

// Decrements minutes and rolls down if needed.
void decMin() {
  // Rolls down if needed.
  if (newMinutes == CLOCK_ROLLDOWN) {
    newMinutes = MAX_MINUTES_AND_SECONDS;
  } else {
    newMinutes -= 1;
  }
}

// Increments seconds and rolls over if needed.
void incSec() {
  newSeconds += 1;
  // Rolls over if needed.
  if (newSeconds == MINUTES_AND_SECONDS_ROLLOVER) {
    newSeconds = 0;
  }
}

// Decrements seconds and rolls down if needed.
void decSec() {
  // Rolls down if needed.
  if (newSeconds == CLOCK_ROLLDOWN) {
    newSeconds = MAX_MINUTES_AND_SECONDS;
  } else {
    newSeconds -= 1;
  }
}

// this fuction will examine where the screen was touched, and perform the
// necessary increment or decrement operation depending on which field was
// touched
void clockDisplay_performIncDec() {
  int16_t x = 0;
  int16_t y = 0;
  uint8_t z = 0;
  display_getTouchedPoint(&x, &y, &z);
  // If the top half of the display was pressed.
  if (y < DISPLAY_Y_CENTER) {
    // Checks if the press was in the first or second thirds of the display.
    if (x < DISPLAY_X_ONE_THIRD) {
      incHours();
    } else if (x < DISPLAY_X_TWO_THIRDS) {
      incMin();
    } else {
      incSec();
    }
  }
  // If the bottom half of the display was pressed.
  else if (y > DISPLAY_Y_CENTER) {
    // Checks if the press was in the first or second thirds of the display
    if (x < DISPLAY_X_ONE_THIRD) {
      decHours();
    } else if (x < DISPLAY_X_TWO_THIRDS) {
      decMin();
    } else {
      decSec();
    }
  }
  clockDisplay_updateTimeDisplay(0);
}

// Adds one second then rolls over accordingly.
void clockDisplay_advanceTimeOneSecond() {
  newSeconds += 1;
  // If seconds is 60, rollover to 0 and add one minute.
  if (newSeconds == MINUTES_AND_SECONDS_ROLLOVER) {
    newSeconds = 0;
    newMinutes += 1;
  }
  // If minutes is 60, rollover to 0 and add one hour.
  if (newMinutes == MINUTES_AND_SECONDS_ROLLOVER) {
    newMinutes = 0;
    newHours += 1;
  }
  // If hours is 13, roll over to 1.
  if (newHours == HOURS_ROLLOVER) {
    newHours = 1;
  }
}

// Increments and decrements hours, minutes, and seconds 8 times each then runs
// the clock fast for 10 seconds.
void clockDisplay_runTest() {
  clockDisplay_init();
  clockDisplay_updateTimeDisplay(1);
  // Increments hours 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    incHours();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Decrements hours 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    decHours();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Increments minutes 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    incMin();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Decrements minutes 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    decMin();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Increments seconds 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    incSec();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Decrements seconds 8 times.
  for (int i = 0; i < CLOCK_TEST_INC_DEC; i++) {
    decSec();
    clockDisplay_updateTimeDisplay(1);
    utils_msDelay(CLOCK_TEST_DELAY);
  }
  // Clock runs at 10 times speed for 10 seconds
  for (int i = 0; i < CLOCK_TEST_TENTH_SECOND; i++) {
    clockDisplay_advanceTimeOneSecond();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(CLOCK_TEST_TENTH_SECOND);
  }
}
