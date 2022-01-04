#include "cardJitsuControl.h"
#include "cardJitsuDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdio.h>

#define ZERO_INIT 0
#define ONE_INIT 1

#define INIT_ST_MSG "init_st\n"
#define HOW_TO_PLAY_ST_MSG "how_to_play_st\n"
#define WAITING_UNTOUCH_MSG "waiting_untouch_st\n"
#define DRAW_BOARD_MSG "draw_board_st\n"
#define START_GAME_MSG "start_game_st\n"
#define PICK_CARD_MSG "pick_card_st\n"
#define COMPARE_AND_SCORE_MSG "compare_and_score_st\n"
#define ADD_CARD_MSG "add_card_st\n"
#define GAME_END_MSG "game_end_st\n"

#define COLOR_FIRE DISPLAY_RED
#define COLOR_WATER DISPLAY_BLUE
#define COLOR_SNOW DISPLAY_WHITE
#define DEFAULT_COLOR DISPLAY_WHITE
#define SMALL_TEXT_SIZE 2

#define FIRE_ELEMENT 1
#define WATER_ELEMENT 2
#define SNOW_ELEMENT 3

#define LARGE_ELEMENT_SIZE 3
#define LEFT_LARGE_ELEMENT_X 50
#define LARGE_ELEMENT_Y 55
#define RIGHT_LARGE_ELEMENT_X 170


uint8_t gameOver = ZERO_INIT;
uint8_t pickedCard = ZERO_INIT;


// Lists the different states in the machine.
enum cardJitsu_st_t {
  init_st,
  how_to_play_st,
  waiting_untouch_st,
  settle_st,
  draw_board_st,
  start_game_st,
  pick_card_st,
  compare_and_score_st,
  add_card_st,
  game_end_st,
} currentState = init_st;

void debugStatePrint() {
  static enum cardJitsu_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentStateVS - this prevents reprinting the same
  // state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                              // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case how_to_play_st:
      printf(HOW_TO_PLAY_ST_MSG);
      break;
    case waiting_untouch_st:
      printf(WAITING_UNTOUCH_MSG);
      break;
    case settle_st:
      printf("settle_st\n");
      break;
    case draw_board_st:
      printf(DRAW_BOARD_MSG);
      break;
    case start_game_st:
      printf(START_GAME_MSG);
      break;
    case pick_card_st:
      printf(PICK_CARD_MSG);
      break;
    case compare_and_score_st:
      printf(COMPARE_AND_SCORE_MSG);
      break;
    case add_card_st:
      printf(ADD_CARD_MSG);
      break;
    case game_end_st:
      printf(GAME_END_MSG);
      break;
    }
  }
}

// Prints a given message on the screen with given color, size, and cursor location.
void cardJitsu_printText(uint32_t color, uint16_t x, uint16_t y, uint8_t size,
                            char msg[]) {
  display_setTextColor(color);
  display_setCursor(x, y);
  display_setTextSize(size);
  display_println(msg);
}

void cardJitsu_printVal(uint32_t color, uint16_t x, uint16_t y, uint8_t size,
                            uint8_t val) {
  display_setTextColor(color);
  display_setCursor(x, y);
  display_setTextSize(size);
  char str1[5];
    sprintf(str1,
            "%d",
            val);
    display_print(str1);

}

#define MAX_RAND_COUNT 1000
#define ELEMENTS_MAX 3
#define VALUES_MAX 10

uint32_t seed = ZERO_INIT;
uint8_t longValueSequence[MAX_RAND_COUNT] = {ZERO_INIT};
uint8_t longElementSequence[MAX_RAND_COUNT] = {ZERO_INIT};
uint8_t cpuElements[MAX_RAND_COUNT] = {ZERO_INIT};
uint8_t cpuValues[MAX_RAND_COUNT] = {ZERO_INIT};
uint16_t currentCard = ZERO_INIT;
uint16_t cpuCount = ZERO_INIT;
uint8_t score = ZERO_INIT;
uint8_t handCard1 = ZERO_INIT;
uint8_t handCard2 = ZERO_INIT;
uint8_t handCard3 = ZERO_INIT;
uint8_t handVal1 = ZERO_INIT;
uint8_t handVal2 = ZERO_INIT;
uint8_t handVal3 = ZERO_INIT;
uint8_t currentElement = ZERO_INIT;
uint8_t currentVal = ZERO_INIT;
uint8_t gameStarted = ZERO_INIT;
uint8_t ADCsettleCounter = ZERO_INIT;
uint8_t playerY = 40;
uint8_t cpuY = 40;
uint8_t playerScore = ZERO_INIT;
uint8_t cpuScore = ZERO_INIT;

// Generates a psudo random sequence.
void generateSequence() {
  srand(seed);
  for (int i = ZERO_INIT; i < MAX_RAND_COUNT; i++) {
    longElementSequence[i] = (rand() % (ELEMENTS_MAX)) + ONE_INIT;
    longValueSequence[i] = (rand() % (VALUES_MAX)) + ONE_INIT;
    cpuElements[i] = (rand() % (ELEMENTS_MAX)) + ONE_INIT;
    cpuValues[i] = (rand() % (VALUES_MAX)) + ONE_INIT;
  }
}

#define WIN_SCORE 1
#define TIE_SCORE 0
#define LOSS_SCORE 2

#define CARD_VAL_X_OFFSET 15
#define CARD_VAL_Y_OFFSET 37

uint8_t compareCards(uint8_t playerElement, uint8_t playerValue, uint8_t cpuElement, uint8_t cpuValue){
  if(playerElement == cpuElement){
    if(playerValue == cpuValue){
      return TIE_SCORE;
    }
    else if(playerValue > cpuValue){
      return WIN_SCORE;
    }
    else{
      return LOSS_SCORE;
    }
  }
  else if((playerElement == FIRE_ELEMENT) && (cpuElement == SNOW_ELEMENT)){
    return WIN_SCORE;
  }
  else if((playerElement == FIRE_ELEMENT) && (cpuElement == WATER_ELEMENT)){
    return LOSS_SCORE;
  }
  else if((playerElement == WATER_ELEMENT) && (cpuElement == FIRE_ELEMENT)){
    return WIN_SCORE;
  }
  else if((playerElement == WATER_ELEMENT) && (cpuElement == SNOW_ELEMENT)){
    return LOSS_SCORE;
  }
  else if((playerElement == SNOW_ELEMENT) && (cpuElement == FIRE_ELEMENT)){
    return LOSS_SCORE;
  }
  else if((playerElement == SNOW_ELEMENT) && (cpuElement == WATER_ELEMENT)){
    return WIN_SCORE;
  }
  else{
    return ZERO_INIT;
  }
}


// Used to init the state machine. Always provided though it may not be
// necessary.
void cardJitsuControl_init(){
  gameOver = ZERO_INIT;
  playerScore = ZERO_INIT;
  cpuScore = ZERO_INIT;
  cpuCount = ZERO_INIT;
  currentCard = ZERO_INIT;
  pickedCard = ZERO_INIT;
  handCard1 = ZERO_INIT;
  handCard2 = ZERO_INIT;
  handCard3 = ZERO_INIT;  
  handVal1 = ZERO_INIT;
  handVal2 = ZERO_INIT;
  handVal3 = ZERO_INIT; 
  currentElement = ZERO_INIT;
  currentVal = ZERO_INIT;
  gameStarted = ZERO_INIT;
  ADCsettleCounter = ZERO_INIT;
  playerY = 40;
  cpuY = 40;
  playerScore = ZERO_INIT;
  cpuScore = ZERO_INIT;
}

uint8_t cardJitsu_getCard() {
  int16_t x, y = ZERO_INIT;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);
  return cardJitsu_computeCardNumber(x, y);
}


// Standard tick function.
void cardJitsuControl_tick(){
  debugStatePrint();
  switch (currentState) {
    case init_st:

      break;
    case how_to_play_st:
      
      break;
    case waiting_untouch_st:
      
      break;
    case settle_st:
    ADCsettleCounter++;
      break;
    case draw_board_st:

      break;
    case start_game_st:

      break;
    case pick_card_st:

      break;
    case compare_and_score_st:

      break;
    case add_card_st:

      break;
    case game_end_st:

      break;
    }

  switch (currentState) {
    case init_st:
      cardJitsuControl_init();
      seed++;
      generateSequence();
      currentState = how_to_play_st;
      break;
    case how_to_play_st:
      cardJitsu_printText(DISPLAY_WHITE, 50, 10, 3, "Welcome to");
      cardJitsu_printText(DISPLAY_WHITE, 50, 40, 3, "Card-Jitsu!");
      cardJitsu_printText(DISPLAY_WHITE, 50, 80, 2, "How to Play:");
      cardJitsu_printText(DISPLAY_WHITE, 50, 100, 1, "Card Jitsu is a card game similar to rock-paper-scissors. You will be given a hand of three cards with elements of fire, water, or snow. Each card is  also given a value. Choose a card to fight against   the cpu. Fire beats snow, snow beats water, and water beats fire. In the case of a tie, whichever value is greater wins. The first player to 5 points wins.    Good luck!");
      cardJitsu_printText(DISPLAY_WHITE, 50, 200, 2, "~~Click to continue~~");
      // utils_msDelay(1000);
      
      currentState = waiting_untouch_st;
      break;
    case waiting_untouch_st:
      // Waits until the display is no longer touched.
      if (display_isTouched()) {
        display_clearOldTouchData();
        currentState = settle_st;
      }
      break;
    case settle_st:
      // waits until the ADC is settled
      if (ADCsettleCounter == 1) {
        ADCsettleCounter = 0;
        if (!display_isTouched()) {    
          if(gameStarted){
              currentState = pick_card_st;
            }
            else{
              display_fillScreen(DISPLAY_BLACK);
              currentState = draw_board_st;
            }
        }
      }
      break;
    case draw_board_st:
      cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_ONE, ZERO_INIT, false);
      cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_TWO, ZERO_INIT, false);
      cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_THREE, ZERO_INIT, false);
      cardJitsu_printText(DISPLAY_WHITE, 10, 10, SMALL_TEXT_SIZE, "CPU:\n");
      cardJitsu_printText(DISPLAY_WHITE, 270, 10, SMALL_TEXT_SIZE, "YOU:\n");
      currentState = start_game_st;
      break;
    case start_game_st:
      cardJitsu_drawElement(CARD_ONE_X, PLAYER_CARD_Y, ONE_INIT, longElementSequence[currentCard], false);
      cardJitsu_drawCardOutline(DISPLAY_RED, CARD_ONE, ONE_INIT, false);
      cardJitsu_printVal(DISPLAY_WHITE, CARD_ONE_X + CARD_VAL_X_OFFSET, PLAYER_CARD_Y + CARD_VAL_Y_OFFSET, SMALL_TEXT_SIZE, longValueSequence[currentCard]);
      handCard1 = longElementSequence[currentCard];
      handVal1 = longValueSequence[currentCard];
      currentCard++;
      
      cardJitsu_drawElement(CARD_TWO_X, PLAYER_CARD_Y, ONE_INIT, longElementSequence[currentCard], false);
      cardJitsu_drawCardOutline(DISPLAY_RED, CARD_TWO, ONE_INIT, false);
      cardJitsu_printVal(DISPLAY_WHITE, CARD_TWO_X + CARD_VAL_X_OFFSET, PLAYER_CARD_Y + CARD_VAL_Y_OFFSET, SMALL_TEXT_SIZE, longValueSequence[currentCard]);
      handCard2 = longElementSequence[currentCard];
      handVal2 = longValueSequence[currentCard];
      currentCard++;

      cardJitsu_drawElement(CARD_THREE_X, PLAYER_CARD_Y, ONE_INIT, longElementSequence[currentCard], false);
      cardJitsu_drawCardOutline(DISPLAY_RED, CARD_THREE, ONE_INIT, false);
      handCard3 = longElementSequence[currentCard];
      handVal3 = longValueSequence[currentCard];
      cardJitsu_printVal(DISPLAY_WHITE, CARD_THREE_X + CARD_VAL_X_OFFSET, PLAYER_CARD_Y + CARD_VAL_Y_OFFSET, SMALL_TEXT_SIZE, longValueSequence[currentCard]);
      currentCard++;

      gameStarted = ONE_INIT;
      currentState = waiting_untouch_st;
      break;
    case pick_card_st:
    display_clearOldTouchData();
      pickedCard = cardJitsu_getCard();
      cpuCount++;
      switch(pickedCard){
        case CARD_ONE:
          currentElement = handCard1;
          currentVal = handVal1;
          break;
        case CARD_TWO:
          currentElement = handCard2;
          currentVal = handVal2;
          break;
        case CARD_THREE:
          currentElement = handCard3;
          currentVal = handVal3;
          break;
      }
      cardJitsu_drawElement(determineCardX(pickedCard), PLAYER_CARD_Y, 1, currentElement, true);
      cardJitsu_drawCardOutline(DISPLAY_BLACK, pickedCard, 1, true);
      cardJitsu_printVal(DISPLAY_BLACK, determineCardX(pickedCard) + CARD_VAL_X_OFFSET, PLAYER_CARD_Y + CARD_VAL_Y_OFFSET, SMALL_TEXT_SIZE, currentVal);
      display_clearOldTouchData();
      
      currentState = compare_and_score_st;
      break;
    case compare_and_score_st:
      #define LEFT_VAL_X 105
      #define RIGHT_VAL_X 225
      #define BIG_VAL_Y 160
      #define COMPARE_DELAY 3000
      cardJitsu_drawElement(LEFT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, cpuElements[cpuCount], false);
      cardJitsu_drawElement(RIGHT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, currentElement, false);
      cardJitsu_printVal(DISPLAY_WHITE, LEFT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, cpuValues[cpuCount]);
      cardJitsu_printVal(DISPLAY_WHITE, RIGHT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, currentVal);
      
      if(compareCards(currentElement, currentVal, cpuElements[cpuCount], cpuValues[cpuCount]) == TIE_SCORE){
        cardJitsu_printText(DISPLAY_WHITE, 150, 100, 4, "=");
        utils_msDelay(COMPARE_DELAY);
        cardJitsu_printText(DISPLAY_BLACK, 150, 100, 4, "=");
        printf("TIEEE\n");
        cardJitsu_drawElement(LEFT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, cpuElements[cpuCount], true);
        cardJitsu_drawElement(RIGHT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, currentElement, true);
        cardJitsu_printVal(DISPLAY_BLACK, LEFT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, cpuValues[cpuCount]);
        cardJitsu_printVal(DISPLAY_BLACK, RIGHT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, currentVal);

        
        currentState = add_card_st;
      }
      else if(compareCards(currentElement, currentVal, cpuElements[cpuCount], cpuValues[cpuCount]) == WIN_SCORE){
        cardJitsu_printText(DISPLAY_WHITE, 150, 100, 4, "<");
        utils_msDelay(COMPARE_DELAY);
        cardJitsu_printText(DISPLAY_BLACK, 150, 100, 4, "<");
        cardJitsu_printText(DISPLAY_WHITE, 300, playerY, SMALL_TEXT_SIZE, "X\n");
        playerY += 30;
        printf("WINNN\n");
        cardJitsu_drawElement(LEFT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, cpuElements[cpuCount], true);
        cardJitsu_drawElement(RIGHT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, currentElement, true);
        cardJitsu_printVal(DISPLAY_BLACK, LEFT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, cpuValues[cpuCount]);
        cardJitsu_printVal(DISPLAY_BLACK, RIGHT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, currentVal);
        playerScore++;
        if(playerScore == 5 || cpuScore == 5){
          printf("GGGGGGGG\n");
          currentState = game_end_st;
          break;
        }
        currentState = add_card_st;
      }
      else if(compareCards(currentElement, currentVal, cpuElements[cpuCount], cpuValues[cpuCount]) == LOSS_SCORE){
        cardJitsu_printText(DISPLAY_WHITE, 150, 100, 4, ">");
        utils_msDelay(COMPARE_DELAY);
        cardJitsu_printText(DISPLAY_BLACK, 150, 100, 4, ">");
        printf("LOSSSS00\n");
        cardJitsu_printText(DISPLAY_WHITE, 10, cpuY, SMALL_TEXT_SIZE, "X\n");
        cpuY += 30;
        cardJitsu_drawElement(LEFT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, cpuElements[cpuCount], true);
        cardJitsu_drawElement(RIGHT_LARGE_ELEMENT_X, LARGE_ELEMENT_Y, LARGE_ELEMENT_SIZE, currentElement, true);
        cardJitsu_printVal(DISPLAY_BLACK, LEFT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, cpuValues[cpuCount]);
        cardJitsu_printVal(DISPLAY_BLACK, RIGHT_VAL_X, BIG_VAL_Y, SMALL_TEXT_SIZE, currentVal);
        cpuScore++;
        if(playerScore == 5 || cpuScore == 5){
          printf("GGGGGGGG\n");
          currentState = game_end_st;
          break;
        }
        currentState = add_card_st;
      }


    
      break;
    case add_card_st:
      if(cardJitsuControl_isGameOver()){
        currentState = game_end_st;
      }
      cardJitsu_drawElement(determineCardX(pickedCard), PLAYER_CARD_Y, 1, longElementSequence[currentCard], false);
      cardJitsu_drawCardOutline(DISPLAY_RED, pickedCard, 1, false);
      cardJitsu_printVal(DISPLAY_WHITE, determineCardX(pickedCard) + CARD_VAL_X_OFFSET, PLAYER_CARD_Y + CARD_VAL_Y_OFFSET, SMALL_TEXT_SIZE, longValueSequence[currentCard]);
      switch(pickedCard){
        case CARD_ONE:
          handCard1 = longElementSequence[currentCard];
          handVal1 = longValueSequence[currentCard];
          break;
        case CARD_TWO:
          handCard2 = longElementSequence[currentCard];
          handVal2 = longValueSequence[currentCard];
          break;
        case CARD_THREE:
          handCard3 = longElementSequence[currentCard];
          handVal3 = longValueSequence[currentCard];
          break;
      }
      pickedCard = ZERO_INIT;
      currentCard++;
      currentState = waiting_untouch_st;
      break;
    case game_end_st:
    display_fillScreen(DISPLAY_BLACK);
    if(playerScore == 5){
      cardJitsu_printText(DISPLAY_WHITE, 70, 80, 4, "You Win!");
    }
    else{
      cardJitsu_printText(DISPLAY_WHITE, 50, 80, 4, "You Lose :(");
    }
      utils_msDelay(3000);
      display_fillScreen(DISPLAY_BLACK);
      seed++;
      generateSequence();
      cardJitsuControl_init();
      currentState = draw_board_st;
      break;
    }










}


// If you want to stop the game after a mistake, check this function after each
// tick.
bool cardJitsuControl_isGameOver(){
  if(gameOver){
    return ONE_INIT;
  }
}

// Use this to set the sequence length. This the sequence set by this function
// will only be recognized when the controller passes through its init state.
void cardJitsuControl_setSequenceLength(uint16_t length);