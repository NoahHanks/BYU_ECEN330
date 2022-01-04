#include "buttons.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define BUTTONS_OFFSET 0x4
#define BUTTONS_BASE_ADDRESS XPAR_PUSH_BUTTONS_BASEADDR
#define BUTTONS_TRI_ADDRESS BUTTONS_BASE_ADDRESS + BUTTONS_OFFSET

#define BUTTON0 0x1
#define BUTTON1 0x2
#define BUTTON2 0x4
#define BUTTON3 0x8
#define BUTTONS_ARE_INPUT 0xF
#define ALL_BUTTONS_PUSHED 0xF

#define TEXT_SIZE 2
#define BUTTON0_TEXT "BTN0"
#define BUTTON1_TEXT "BTN1"
#define BUTTON2_TEXT "BTN2"
#define BUTTON3_TEXT "BTN3"

#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 120

#define BUTTON_RECT_Y_CURSOR 0
#define BUTTON0_RECT_X_CURSOR 240
#define BUTTON1_RECT_X_CURSOR 160
#define BUTTON2_RECT_X_CURSOR 80
#define BUTTON3_RECT_X_CURSOR 0

#define BUTTON_TEXT_Y_CURSOR 50
#define BUTTON0_TEXT_X_CURSOR 260
#define BUTTON1_TEXT_X_CURSOR 180
#define BUTTON2_TEXT_X_CURSOR 100
#define BUTTON3_TEXT_X_CURSOR 20

// Returns the current value of the buttons from the GPIO register when using
// the offset.
int32_t buttons_readGpioRegister(int32_t offset) {
  return Xil_In32(BUTTONS_BASE_ADDRESS +
                  offset); // BUTTONS_GPIO_DEVICE_BASE_ADDRESS
}

// Writes to the values of the buttons using the GPIO register. Also uses
// the same offset then returns the value.
int32_t buttons_writeGpioRegister(int32_t offset, int32_t value) {}

// Initializes the button driver software and hardware. Returns one of the
// defined status values (above).
int32_t buttons_init() {
  Xil_Out32(BUTTONS_BASE_ADDRESS, BUTTONS_ARE_INPUT);
  return BUTTONS_INIT_STATUS_OK;
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() { return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR); }

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest() {
  buttons_init();
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  display_setTextSize(TEXT_SIZE);
  uint32_t buttonState = 0;    // Newer button state.
  uint32_t oldButtonState = 0; // Previous button state.
  // Checks to see if all the buttons are pressed or not.
  while (buttonState ^ ALL_BUTTONS_PUSHED) {
    oldButtonState = buttonState; // Sets the previous button state the what the
                                  // newer one is.
    buttonState = buttons_read(); // Reads the newest button input.
    // Checks to see if the button was not pressed then was just pressed. Draws
    // the yellow rectangle for button 0, sets the text color to black,
    // potitions the cursor to the text location, then outputs the text for
    // button 0.
    if ((buttonState & BUTTON0) && !(oldButtonState & BUTTON0)) {
      display_fillRect(BUTTON0_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_YELLOW);
      display_setTextColor(DISPLAY_BLACK);
      display_setCursor(BUTTON0_TEXT_X_CURSOR, BUTTON_TEXT_Y_CURSOR);
      display_println(BUTTON0_TEXT);
    } else if (!(buttonState & BUTTON0) && (oldButtonState & BUTTON0)) {
      display_fillRect(BUTTON0_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);
    }
    // Checks to see if the button was not pressed then was just pressed. Draws
    // the green rectangle for button 1, sets the text color to black, potitions
    // the cursor to the text location, then outputs the text for button 1.
    if ((buttonState & BUTTON1) && !(oldButtonState & BUTTON1)) {
      display_fillRect(BUTTON1_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_GREEN);
      display_setTextColor(DISPLAY_BLACK);
      display_setCursor(BUTTON1_TEXT_X_CURSOR, BUTTON_TEXT_Y_CURSOR);
      display_println(BUTTON1_TEXT);
    } else if (!(buttonState & BUTTON1) && (oldButtonState & BUTTON1)) {
      display_fillRect(BUTTON1_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);
    }
    // Checks to see if the button was not pressed then was just pressed. Draws
    // the red rectangle for button 2, sets the text color to white, potitions
    // the cursor to the text location, then outputs the text for button 2.
    if ((buttonState & BUTTON2) && !(oldButtonState & BUTTON2)) {
      display_fillRect(BUTTON2_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_RED);
      display_setTextColor(DISPLAY_WHITE);
      display_setCursor(BUTTON2_TEXT_X_CURSOR, BUTTON_TEXT_Y_CURSOR);
      display_println(BUTTON2_TEXT);
    } else if (!(buttonState & BUTTON2) && (oldButtonState & BUTTON2)) {
      display_fillRect(BUTTON2_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);
    }
    // Checks to see if the button was not pressed then was just pressed. Draws
    // the blue rectangle for button 3, sets the text color to white, potitions
    // the cursor to the text location, then outputs the text for button 3.
    if ((buttonState & BUTTON3) && !(oldButtonState & BUTTON3)) {
      display_fillRect(BUTTON3_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLUE);
      display_setTextColor(DISPLAY_WHITE);
      display_setCursor(BUTTON3_TEXT_X_CURSOR, BUTTON_TEXT_Y_CURSOR);
      display_println(BUTTON3_TEXT);
    } else if (!(buttonState & BUTTON3) && (oldButtonState & BUTTON3)) {
      display_fillRect(BUTTON3_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR,
                       BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);
    }
  }
  display_fillRect(BUTTON0_RECT_X_CURSOR, BUTTON_RECT_Y_CURSOR, BUTTON_WIDTH,
                   BUTTON_HEIGHT, DISPLAY_BLACK);
  display_fillScreen(
      DISPLAY_BLACK); // Turns the screen black after pressing all the buttons.
}