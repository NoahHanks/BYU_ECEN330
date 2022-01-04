#include "simonDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1

#define SCREEN_DIVIDER 2
#define SCREEN_WIDTH_HALF DISPLAY_WIDTH / 2
#define SCREEN_WIDTH_EIGHTH DISPLAY_WIDTH / 8
#define SCREEN_HEIGHT_HALF DISPLAY_HEIGHT / 2
#define SCREEN_HEIGHT_EIGHTH DISPLAY_HEIGHT / 8
#define X_OFFSET 30

// Takes the input from the coordinates of the touch and determines which
// region of the screen is being pressed.
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y) {
  if (x < SCREEN_WIDTH_HALF) {
    if (y < SCREEN_HEIGHT_HALF) {
      return SIMON_DISPLAY_REGION_0;
    } else {
      return SIMON_DISPLAY_REGION_2;
    }
  } else {
    if (y < SCREEN_HEIGHT_HALF) {
      return SIMON_DISPLAY_REGION_1;
    } else {
      return SIMON_DISPLAY_REGION_3;
    }
  }
}

// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase) {
  if (!erase) {
    if (regionNumber == SIMON_DISPLAY_REGION_0) {
      display_fillRect(SCREEN_WIDTH_EIGHTH, SCREEN_HEIGHT_EIGHTH,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_RED);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_1) {
      display_fillRect((SCREEN_WIDTH_EIGHTH + SCREEN_WIDTH_HALF + X_OFFSET),
                       SCREEN_HEIGHT_EIGHTH, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_YELLOW);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_2) {
      display_fillRect(SCREEN_WIDTH_EIGHTH,
                       (SCREEN_HEIGHT_HALF + SCREEN_HEIGHT_EIGHTH),
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_BLUE);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_3) {
      display_fillRect((SCREEN_WIDTH_EIGHTH + SCREEN_WIDTH_HALF + X_OFFSET),
                       (SCREEN_HEIGHT_HALF + SCREEN_HEIGHT_EIGHTH),
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_GREEN);
    }
  } else {
    if (regionNumber == SIMON_DISPLAY_REGION_0) {
      display_fillRect(SCREEN_WIDTH_EIGHTH, SCREEN_HEIGHT_EIGHTH,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_BLACK);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_1) {
      display_fillRect((SCREEN_WIDTH_EIGHTH + SCREEN_WIDTH_HALF + X_OFFSET),
                       SCREEN_HEIGHT_EIGHTH, SIMON_DISPLAY_BUTTON_WIDTH,
                       SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_2) {
      display_fillRect(SCREEN_WIDTH_EIGHTH,
                       (SCREEN_HEIGHT_HALF + SCREEN_HEIGHT_EIGHTH),
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_BLACK);
    }
    if (regionNumber == SIMON_DISPLAY_REGION_3) {
      display_fillRect((SCREEN_WIDTH_EIGHTH + SCREEN_WIDTH_HALF + X_OFFSET),
                       (SCREEN_HEIGHT_HALF + SCREEN_HEIGHT_EIGHTH),
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DISPLAY_BLACK);
    }
  }
}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, false);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, false);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, false);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, false);
}

// Convenience function that erases all of the buttons.
void simonDisplay_eraseAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, true);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, true);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, true);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, true);
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to
// "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase) {
  if (!erase) {
    if (regionNo == SIMON_DISPLAY_REGION_0) {
      display_fillRect(0, 0, SIMON_DISPLAY_SQUARE_WIDTH,
                       SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_RED);
    }
    if (regionNo == SIMON_DISPLAY_REGION_1) {
      display_fillRect(SCREEN_WIDTH_HALF + X_OFFSET, 0,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DISPLAY_YELLOW);
    }
    if (regionNo == SIMON_DISPLAY_REGION_2) {
      display_fillRect(0, SCREEN_HEIGHT_HALF, SIMON_DISPLAY_SQUARE_WIDTH,
                       SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLUE);
    }
    if (regionNo == SIMON_DISPLAY_REGION_3) {
      display_fillRect(SCREEN_WIDTH_HALF + X_OFFSET, SCREEN_HEIGHT_HALF,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DISPLAY_GREEN);
    }
  } else {
    if (regionNo == SIMON_DISPLAY_REGION_0) {
      display_fillRect(0, 0, SIMON_DISPLAY_SQUARE_WIDTH,
                       SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
    }
    if (regionNo == SIMON_DISPLAY_REGION_1) {
      display_fillRect(SCREEN_WIDTH_HALF + X_OFFSET, 0,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DISPLAY_BLACK);
    }
    if (regionNo == SIMON_DISPLAY_REGION_2) {
      display_fillRect(0, SCREEN_HEIGHT_HALF, SIMON_DISPLAY_SQUARE_WIDTH,
                       SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
    }
    if (regionNo == SIMON_DISPLAY_REGION_3) {
      display_fillRect(SCREEN_WIDTH_HALF + X_OFFSET, SCREEN_HEIGHT_HALF,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DISPLAY_BLACK);
    }
  }
}

#define TEXT_SIZE 2
#define MAX_STRING 255
#define UTILS_DELAY 60

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void simonDisplay_runTest(uint16_t touchCount) {
  char str[MAX_STRING];
  uint8_t regionNumber;
  uint8_t touchesCount = ZERO_INIT;
  display_fillScreen(DISPLAY_BLACK);
  // checks that the display is being touched.
  while (display_isTouched()) {
    simonDisplay_drawAllButtons();
    bool screenTouched = false;
    int16_t x, y;
    uint8_t z;
    // loops until enough buttons have been pressed.
    while (touchesCount < touchCount) {
      // checks if the display is currently being touched or not.
      if (!display_isTouched() && screenTouched) {
        simonDisplay_drawSquare(regionNumber, false);
        simonDisplay_drawButton(regionNumber, false);
        screenTouched = false;
      } else if (display_isTouched() && !screenTouched) {
        screenTouched = true;
        touchesCount++;
        display_clearOldTouchData();
        utils_msDelay(UTILS_DELAY);
        display_getTouchedPoint(&x, &y, &z);
        regionNumber = simonDisplay_computeRegionNumber(x, y);
        simonDisplay_drawSquare(regionNumber, false);
      }
    }
    break;
  }
}