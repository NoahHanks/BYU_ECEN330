#ifndef CARDJITSUDISPLAY_H_
#define CARDJITSUDISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

#define CARD_WIDTH 40
#define CARD_HEIGHT 55

#define CARD_ONE 1
#define CARD_TWO 2
#define CARD_THREE 3

#define CARD_ONE_X 100
#define CARD_TWO_X 150
#define CARD_THREE_X 200

#define FIRE 1
#define WATER 2
#define SNOW 3

#define PLAYER_CARD_Y DISPLAY_HEIGHT - CARD_HEIGHT

// Determines the X cursor location when given the card number.
uint16_t determineCardX(uint8_t card);

// Determines which card was selected.
int8_t cardJitsu_computeCardNumber(int16_t x, int16_t y);

// Draws the outlines of the cards.
void cardJitsu_drawCardOutline(int color, uint8_t card, uint8_t player, bool erase);

// Draws the element on the display.
void cardJitsu_drawElement(uint16_t x, uint16_t y, uint16_t size, uint8_t element, bool erase);

// Convenience function that erases all of the cards.
void cardJitsu_eraseAllCards();

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void cardJitsu_runTest(uint16_t touchCount);

#endif /* CARDJITSUDISPLAY_H_ */
 