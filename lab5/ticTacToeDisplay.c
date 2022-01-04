#include "ticTacToeDisplay.h"
#include "buttons.h"
#include "display.h"
#include "switches.h"
#include "utils.h"
#include <stdio.h>

#define TOP_LFT_ROW_COL 0
#define MID_ROW_COL 1
#define END_ROW_COL 2
#define ZERO_INITIALIZER 0

#define X_THIRD (DISPLAY_WIDTH / 3)
#define X_SIXTH (DISPLAY_WIDTH / 6)
#define X_TWELFTH (DISPLAY_WIDTH / 12)

#define Y_THIRD (DISPLAY_HEIGHT / 3)
#define Y_SIXTH (DISPLAY_HEIGHT / 6)
#define Y_TWELFTH (DISPLAY_HEIGHT / 12)
#define Y_SIXTEENTH (DISPLAY_HEIGHT / 16)

#define O_RADIUS 30

#define TOUCH_SCREEN_DELAY 200
#define TEXT_SIZE 2
#define SWITCH_0_MASK 0x01
#define BUTTON_0_MASK 0x01
#define BUTTON_1_MASK 0x02
#define END_TEST_TEXT "Terminating Test"

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  ticTacToeDisplay_drawBoardLines();
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase) {
  // draw a green X if we're not erasing or black otherwise.
  if (!erase) {
    display_drawLine((X_THIRD * row) + X_TWELFTH,
                     (Y_THIRD * column) + Y_SIXTEENTH,
                     (X_THIRD * (row + 1)) - X_TWELFTH,
                     (Y_THIRD * (column + 1)) - Y_SIXTEENTH, DISPLAY_GREEN);
    display_drawLine((X_THIRD * (row)) + X_TWELFTH,
                     (Y_THIRD * (column + 1)) - Y_SIXTEENTH,
                     (X_THIRD * (row + 1)) - X_TWELFTH,
                     (Y_THIRD * column) + Y_SIXTEENTH, DISPLAY_GREEN);
  } else {
    display_drawLine((X_THIRD * row) + X_TWELFTH,
                     (Y_THIRD * column) + Y_SIXTEENTH,
                     (X_THIRD * (row + 1)) - X_TWELFTH,
                     (Y_THIRD * (column + 1)) - Y_SIXTEENTH, DISPLAY_BLACK);
    display_drawLine((X_THIRD * (row)) + X_TWELFTH,
                     (Y_THIRD * (column + 1)) - Y_SIXTEENTH,
                     (X_THIRD * (row + 1)) - X_TWELFTH,
                     (Y_THIRD * column) + Y_SIXTEENTH, DISPLAY_BLACK);
  }
}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase) {
  int16_t x = (X_THIRD * row) + X_SIXTH;
  int16_t y = (Y_THIRD * column) + Y_SIXTH;
  // draw a green O if we're not erasing or black otherwise.
  if (!erase) {
    display_drawCircle(x, y, O_RADIUS, DISPLAY_GREEN);
  } else {
    display_drawCircle(x, y, O_RADIUS, DISPLAY_BLACK);
  }
}

// After a touch has been detected and after the proper delay, this sets the row
// and column arguments according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t *row,
                                                       uint8_t *column) {
  int16_t x;
  int16_t y;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);
  //  checks the y of the first third.
  if (y <= Y_THIRD) {
    *column = TOP_LFT_ROW_COL;
  }
  // checks the y of the second third.
  else if (y <= Y_THIRD * END_ROW_COL) {
    *column = MID_ROW_COL;
  }
  // assumes y is in the third column otherwise.
  else {
    *column = END_ROW_COL;
  }

  //  checks the x of the first third.
  if (x <= X_THIRD) {
    *row = TOP_LFT_ROW_COL;
  }
  //  checks the x of the second third.
  else if (x <= X_THIRD * END_ROW_COL) {
    *row = MID_ROW_COL;
  }
  // assumes x is in the third row otherwise.
  else {
    *row = END_ROW_COL;
  }
}

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will
// paint an X or an O, depending on whether switch 0 (SW0) is slid up (O) or
// down (X). When BTN0 is pushed, the screen is cleared. The test terminates
// when BTN1 is pushed.
void ticTacToeDisplay_runTest() {
  ticTacToeDisplay_init();
  switches_init();
  buttons_init();
  display_setCursor(TOP_LFT_ROW_COL, TOP_LFT_ROW_COL);
  display_setTextColor(DISPLAY_WHITE);
  display_setTextSize(TEXT_SIZE);
  uint8_t row;
  uint8_t column;

  // keep looping until button 1 is pressed
  while (1) {
    // if the display is touched
    if (display_isTouched()) {
      display_clearOldTouchData();
      utils_msDelay(TOUCH_SCREEN_DELAY);
      ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);
      // draws O if switch 0 is on, otherwise draw X
      if (switches_read() & SWITCH_0_MASK) {
        ticTacToeDisplay_drawO(row, column, false);
      } else {
        ticTacToeDisplay_drawX(row, column, false);
      }
    }
    // clear display if button 0 is pressed
    else if (buttons_read() & BUTTON_0_MASK) {
      display_fillScreen(DISPLAY_BLACK);
      ticTacToeDisplay_drawBoardLines();
    }
    // break if button 1 is pressed
    else if (buttons_read() & BUTTON_1_MASK) {
      display_fillScreen(DISPLAY_BLACK);
      display_println(END_TEST_TEXT);
      break;
    }
  }
}

// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines() {
  display_drawFastVLine(X_THIRD, ZERO_INITIALIZER, DISPLAY_HEIGHT,
                        DISPLAY_GREEN);
  display_drawFastVLine(X_THIRD * END_ROW_COL, ZERO_INITIALIZER, DISPLAY_HEIGHT,
                        DISPLAY_GREEN);
  display_drawFastHLine(ZERO_INITIALIZER, Y_THIRD, DISPLAY_WIDTH,
                        DISPLAY_GREEN);
  display_drawFastHLine(ZERO_INITIALIZER, Y_THIRD * END_ROW_COL, DISPLAY_WIDTH,
                        DISPLAY_GREEN);
}
