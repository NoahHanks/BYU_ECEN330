#include "cardJitsuDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1










// Determines which card was selected.
int8_t cardJitsu_computeCardNumber(int16_t x, int16_t y){
    if (y > (PLAYER_CARD_Y)) {
        if (x < CARD_TWO_X) {
            return CARD_ONE;
        }
        else if((x > CARD_ONE_X) & (x < CARD_THREE_X)){
            return CARD_TWO;
        }
        else if(x > CARD_TWO_X){
            return CARD_THREE;
        }
    }
    else{
        return -1;
    }
}

uint16_t determineCardX(uint8_t card){
    if(card == CARD_ONE){
        return CARD_ONE_X;
    }
    else if(card == CARD_TWO){
        return CARD_TWO_X;
    }
    else if(card == CARD_THREE){
        return CARD_THREE_X;
    }
}

// Draws the outlines of the cards.
void cardJitsu_drawCardOutline(int color, uint8_t card, uint8_t player, bool erase){
    uint16_t cardX = determineCardX(card);
    if(player){
        if(erase){
            display_drawRect(cardX, PLAYER_CARD_Y, CARD_WIDTH, CARD_HEIGHT, DISPLAY_BLACK);
        }
        else{
            display_drawRect(cardX, PLAYER_CARD_Y, CARD_WIDTH, CARD_HEIGHT, color);
        }
    }
    else{
        if(erase){
            display_drawRect(cardX, ZERO_INIT, CARD_WIDTH, CARD_HEIGHT, DISPLAY_BLACK);
        }
        else{
            display_drawRect(cardX, ZERO_INIT, CARD_WIDTH, CARD_HEIGHT, color);
        }
    }

}

// Draws the element on the display.
void cardJitsu_drawElement(uint16_t x, uint16_t y, uint16_t size, uint8_t element, bool erase){
    int color;
    if(erase){
        color = DISPLAY_BLACK;
    }
    else{
        if(element == FIRE){
            color = DISPLAY_RED;
        }
        else if(element == WATER){
            color = DISPLAY_BLUE;
        }
        else if(element == SNOW){
            color = DISPLAY_WHITE;
        }
    }

    #define FIRE_X_OFFSET_LEVEL_1 10
    #define FIRE_X_OFFSET_LEVEL_2 15
    #define FIRE_X_OFFSET_LEVEL_3 20
    #define FIRE_X_OFFSET_LEVEL_4 25
    #define FIRE_X_OFFSET_LEVEL_5 30

    #define FIRE_Y_OFFSET_LEVEL_1 7
    #define FIRE_Y_OFFSET_LEVEL_2 13
    #define FIRE_Y_OFFSET_LEVEL_3 20
    #define FIRE_Y_OFFSET_LEVEL_4 30

    if(element == FIRE){
        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_2 * size), y+(FIRE_Y_OFFSET_LEVEL_4 * size), x+(FIRE_X_OFFSET_LEVEL_4 * size), y+(FIRE_Y_OFFSET_LEVEL_4 * size), color);
        
        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_2 * size), y+(FIRE_Y_OFFSET_LEVEL_4 * size), x+(FIRE_X_OFFSET_LEVEL_1 * size), y+(FIRE_Y_OFFSET_LEVEL_2 * size), color);
        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_4 * size), y+(FIRE_Y_OFFSET_LEVEL_4 * size), x+(FIRE_X_OFFSET_LEVEL_5 * size), y+(FIRE_Y_OFFSET_LEVEL_2 * size), color);

        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_1 * size), y+(FIRE_Y_OFFSET_LEVEL_2 * size), x+(FIRE_X_OFFSET_LEVEL_2 * size), y+(FIRE_Y_OFFSET_LEVEL_3 * size), color);
        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_5 * size), y+(FIRE_Y_OFFSET_LEVEL_2 * size), x+(FIRE_X_OFFSET_LEVEL_4 * size), y+(FIRE_Y_OFFSET_LEVEL_3 * size), color);

        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_3 * size), y+(FIRE_Y_OFFSET_LEVEL_1 * size), x+(FIRE_X_OFFSET_LEVEL_2 * size), y+(FIRE_Y_OFFSET_LEVEL_3 * size), color);
        display_drawLine(x+(FIRE_X_OFFSET_LEVEL_3 * size), y+(FIRE_Y_OFFSET_LEVEL_1 * size), x+(FIRE_X_OFFSET_LEVEL_4 * size), y+(FIRE_Y_OFFSET_LEVEL_3 * size), color);
    }

    #define WATER_X_OFFSET_TOP_FAR_LEFT 10 
    #define WATER_X_OFFSET_TOP_MID_LEFT 15
    #define WATER_X_OFFSET_TOP_MID_RIGHT 25
    #define WATER_X_OFFSET_TOP_FAR_RIGHT 30

    #define WATER_Y_OFFSET_LEVEL_1 10
    #define WATER_Y_OFFSET_LEVEL_2 15
    #define WATER_Y_OFFSET_LEVEL_3 20
    #define WATER_Y_OFFSET_LEVEL_4 25
    #define WATER_Y_OFFSET_LEVEL_5 30

    if(element == WATER){
        display_drawLine(x+(WATER_X_OFFSET_TOP_FAR_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_2 * size), x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_1 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_1 * size), x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), x+(WATER_X_OFFSET_TOP_FAR_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_2 * size), color);

        display_drawLine(x+(WATER_X_OFFSET_TOP_FAR_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_2 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_2 * size), x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_4 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_4 * size), x+(WATER_X_OFFSET_TOP_FAR_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), color);
        
        display_drawLine(x+(WATER_X_OFFSET_TOP_FAR_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_4 * size), x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_LEFT * size), y+(WATER_Y_OFFSET_LEVEL_3 * size), x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_5 * size), color);
        display_drawLine(x+(WATER_X_OFFSET_TOP_MID_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_5 * size), x+(WATER_X_OFFSET_TOP_FAR_RIGHT * size), y+(WATER_Y_OFFSET_LEVEL_4 * size), color);
        
    }

    #define SNOW_X_OFFSET_MID 20
    #define SNOW_X_OFFSET_LEFT 10
    #define SNOW_X_OFFSET_RIGHT 30

    #define SNOW_Y_OFFSET_TOP 10
    #define SNOW_Y_OFFSET_MID_TOP 15
    #define SNOW_Y_OFFSET_MID_BOT 25
    #define SNOW_Y_OFFSET_BOT 30

    if(element == SNOW){
        display_drawLine(x+(SNOW_X_OFFSET_MID * size), y+(SNOW_Y_OFFSET_TOP * size), x+(SNOW_X_OFFSET_MID * size), y+(SNOW_Y_OFFSET_BOT * size), color);
        display_drawLine(x+(SNOW_X_OFFSET_LEFT * size), y+(SNOW_Y_OFFSET_MID_TOP * size), x+(SNOW_X_OFFSET_RIGHT * size), y+(SNOW_Y_OFFSET_MID_BOT * size), color);
        display_drawLine(x+(SNOW_X_OFFSET_LEFT * size), y+(SNOW_Y_OFFSET_MID_BOT * size), x+(SNOW_X_OFFSET_RIGHT * size), y+(SNOW_Y_OFFSET_MID_TOP * size), color);
    }

}

// Convenience function that erases all of the cards.
void cardJitsu_eraseAllCards(){
    for(int i = ZERO_INIT; i < WATER; i++){
            for(int j = ZERO_INIT; j <= SNOW; j++){
                cardJitsu_drawCardOutline(DISPLAY_BLACK, j, i, true);
            }
        }
}

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void cardJitsu_runTest(uint16_t touchCount){



}