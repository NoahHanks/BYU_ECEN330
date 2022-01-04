/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Print out "hello world" on both the console and the LCD screen.

#include "display.h"

#define TEXT_SIZE 2
#define CURSOR_X 0
#define CURSOR_Y 0
#define CENTER_X 160 //
#define RIGHT_TRIANGLE_X 190
#define LEFT_TRIANGLE_X 130
#define HIGHEST_TRIANGLE_Y 30
#define TIP_TOP_TRIANGLE_Y 90
#define LOWEST_TRIANGLE_Y 210
#define TIP_BOT_TRIANGLE_Y 150

#define RADIUS 30
#define LEFT_CIRCLE_X 80
#define LEFT_CIRCLE_Y 120
#define RIGHT_CIRCLE_X 240
#define RIGHT_CIRCLE_Y 120

#define DISPLAY_ORANGE 0xF0A5

#include <stdio.h>
int main() {
  display_init(); // Must init all of the software and underlying hardware for
                  // LCD.
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_setCursor(CURSOR_X, CURSOR_Y); // The upper left of the LCD screen.
  display_setTextColor(DISPLAY_RED);     // Make the text red.
  display_setTextSize(TEXT_SIZE);        // Make the text a little larger.
  display_drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
  display_drawLine(0, DISPLAY_HEIGHT, DISPLAY_WIDTH, 0, DISPLAY_GREEN);
  display_fillTriangle(CENTER_X, TIP_TOP_TRIANGLE_Y, RIGHT_TRIANGLE_X,
                       HIGHEST_TRIANGLE_Y, LEFT_TRIANGLE_X, HIGHEST_TRIANGLE_Y,
                       DISPLAY_YELLOW);
  display_drawTriangle(CENTER_X, TIP_BOT_TRIANGLE_Y, RIGHT_TRIANGLE_X,
                       LOWEST_TRIANGLE_Y, LEFT_TRIANGLE_X, LOWEST_TRIANGLE_Y,
                       DISPLAY_YELLOW);
  display_drawCircle(LEFT_CIRCLE_X, LEFT_CIRCLE_Y, RADIUS, DISPLAY_ORANGE);
  display_fillCircle(RIGHT_CIRCLE_X, RIGHT_CIRCLE_Y, RADIUS, DISPLAY_RED);

  printf("hello world!"); // This prints on the console.
  return 0;
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in
// supportFiles).
void isr_function() {
  // Empty for now.
}