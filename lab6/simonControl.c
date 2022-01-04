#include "buttonHandler.h"
#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "utils.h"
#include "verifySequence.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define ZERO_INIT 0
#define ONE_INIT 1

#define SIMON_MSG "SIMON\n"
#define TOUCH_TO_PLAY_MSG "Touch to play\n\n        Length: 4"
#define YAY_MSG "Yay!"
#define TRY_AGAIN_MSG "Touch to try again"
#define LAST_SEQUENCE_MSG "Last Sequence: "

#define INIT_ST_MSG "sc_init_st\n"
#define WAITING_UNTOUCH_ST_MSG "sc_waiting_untouch_st\n"
#define START_SC_ST_MSG "sc_start_sc_st\n"
#define ENABLE_FS_ST_MSG "sc_enable_fs_st\n"
#define WAITING_FS_ST_MSG "sc_waiting_fs_st\n"
#define DISABLE_FS_ST_MSG "sc_disable_fs_st\n"
#define ENABLE_VS_ST_MSG "sc_enable_vs_st\n"
#define WAITING_VS_ST_MSG "sc_waiting_vs_st\n"
#define DISABLE_VS_ST_MSG "sc_disable_vs_st\n"
#define START_END_GAME_ST_MSG "sc_start_end_game_st\n"
#define WAITING_END_GAME_ST_MSG "sc_waiting_end_game_st\n"
#define START_YAY_ST_MSG "sc_start_yay_st\n"
#define WAITING_YAY_ST_MSG "sc_waiting_yay_st\n"
#define START_NEW_LEVEL_ST_MSG "sc_start_new_level_st\n"
#define WAITING_NEW_LEVEL_ST_MSG "sc_waiting_new_level_st\n"

#define TEXT_SIZE_SMALL 2
#define TEXT_SIZE_LARGE 5
#define YAY_DURATION 40
#define FIRST_SEQUENCE 4

#define TRY_AGAIN_MSG_X_COORDINATE 50
#define TOUCH_TO_PLAY_MSG_X_COORDINATE 70
#define SIMON_MSG_X_COORDINATE 80
#define YAY_MSG_X_COORDINATE 120
#define SEQUENCE_MSG_X_COORDINATE 250

#define MSG_Y_COORDINATE 90
#define TOUCH_TO_PLAY_MSG_Y_COORDINATE 140

uint8_t initDone = ZERO_INIT;
uint32_t seed = ZERO_INIT;
uint16_t currentSequenceLength = FIRST_SEQUENCE;
uint8_t levelLength = FIRST_SEQUENCE;
uint16_t currentLevel = ONE_INIT;
uint8_t longSequence[GLOBALS_MAX_FLASH_SEQUENCE] = {ZERO_INIT};
uint8_t wrongMove = ZERO_INIT;
uint8_t yayCounter = ZERO_INIT;
uint8_t endCounter = ZERO_INIT;
uint8_t newLevelCounter = ZERO_INIT;
uint8_t resetGame = ZERO_INIT;
uint8_t newSequence = ZERO_INIT;

// All the states of the machine.
enum simonControl_st_t {
  init_st,
  waiting_untouch_st,
  start_sc_st,
  enable_fs_st,
  waiting_fs_st,
  disable_fs_st,
  enable_vs_st,
  waiting_vs_st,
  disable_vs_st,
  start_end_game_st,
  waiting_end_game_st,
  start_yay_st,
  waiting_yay_st,
  start_new_level_st,
  waiting_new_level_st

} currentStateSC = init_st;

void debugStatePrintSC() {
  static enum simonControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentStateVS - this prevents reprinting the same
  // state name over and over.
  if (previousState != currentStateSC || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentStateSC;       // keep track of the last state that you were in.
    switch (currentStateSC) { // This prints messages based upon the state that
                              // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case waiting_untouch_st:
      printf(WAITING_UNTOUCH_ST_MSG);
      break;
    case start_sc_st:
      printf(START_SC_ST_MSG);
      break;
    case enable_fs_st:
      printf(ENABLE_FS_ST_MSG);
      break;
    case waiting_fs_st:
      printf(WAITING_FS_ST_MSG);
      break;
    case disable_fs_st:
      printf(DISABLE_FS_ST_MSG);
      break;
    case enable_vs_st:
      printf(ENABLE_VS_ST_MSG);
      break;
    case waiting_vs_st:
      printf(WAITING_VS_ST_MSG);
      break;
    case disable_vs_st:
      printf(DISABLE_VS_ST_MSG);
      break;
    case start_end_game_st:
      printf(START_END_GAME_ST_MSG);
      break;
    case waiting_end_game_st:
      printf(WAITING_END_GAME_ST_MSG);
      break;
    case start_yay_st:
      printf(START_YAY_ST_MSG);
      break;
    case waiting_yay_st:
      printf(WAITING_YAY_ST_MSG);
      break;
    case start_new_level_st:
      printf(START_NEW_LEVEL_ST_MSG);
      break;
    case waiting_new_level_st:
      printf(WAITING_NEW_LEVEL_ST_MSG);
      break;
    }
  }
}

void simonControl_init() {
  currentSequenceLength = FIRST_SEQUENCE;
  currentLevel = ONE_INIT;
  yayCounter = ZERO_INIT;
  endCounter = ZERO_INIT;
  newLevelCounter = ZERO_INIT;
  initDone = ZERO_INIT;
  resetGame = ZERO_INIT;
  newSequence = ZERO_INIT;
  display_setTextColor(DISPLAY_WHITE);
}

// Enables the control state machine.
void simonControl_enable() {}

// Disables the controll state machine.
void simonControl_disable() {}

// If you want to stop the game after a mistake, check this function after each
// tick.
bool simonControl_isGameOver() {}

// Takes input of the color, cursor, text size and message then prints it on the
// display.
void simonControl_printText(uint32_t color, uint8_t x, uint8_t y, uint8_t size,
                            char msg[]) {
  display_setTextColor(color);
  display_setCursor(x, y);
  display_setTextSize(size);
  display_println(msg);
}

#define BLACK_SQUARE_X 200
#define BLACK_SQUARE_Y 60
#define BLACK_SQUARE_W 100
#define BLACK_SQUARE_H 50
// Draws a black sqaure over the sequence indicator.
void simonControl_blackSquare() {
  display_fillRect(BLACK_SQUARE_X, BLACK_SQUARE_Y, BLACK_SQUARE_W,
                   BLACK_SQUARE_H, DISPLAY_BLACK);
}

// Generates a psudo random sequence.
void generateSequence() {
  srand(seed);
  for (int i = 0; i < currentSequenceLength; i++) {
    longSequence[i] = rand() % (FIRST_SEQUENCE);
  }
}

// Standard tick function.
void simonControl_tick() {
  debugStatePrintSC();
  switch (currentStateSC) {
  // state actions
  case init_st:
    if (!initDone) {
      // checks if the game needs to be reset or not.
      if (!resetGame) {
        simonControl_blackSquare();
        simonControl_printText(DISPLAY_WHITE, SIMON_MSG_X_COORDINATE,
                               MSG_Y_COORDINATE, TEXT_SIZE_LARGE, SIMON_MSG);
        simonControl_printText(DISPLAY_WHITE, TOUCH_TO_PLAY_MSG_X_COORDINATE,
                               TOUCH_TO_PLAY_MSG_Y_COORDINATE, TEXT_SIZE_SMALL,
                               TOUCH_TO_PLAY_MSG);
        resetGame = ONE_INIT;
      }
      initDone = ONE_INIT;
    }
    seed++;
    break;
  case waiting_untouch_st:
    break;
  case start_sc_st:
    globals_setSequenceIterationLength(currentLevel);
    break;
  case enable_fs_st:
    flashSequence_enable();
    break;
  case waiting_fs_st:
    break;
  case disable_fs_st:
    flashSequence_disable();
    break;
  case enable_vs_st:
    verifySequence_enable();
    break;
  case waiting_vs_st:
    break;
  case disable_vs_st:
    verifySequence_disable();
    break;
  case start_yay_st:
    simonControl_printText(DISPLAY_WHITE, YAY_MSG_X_COORDINATE,
                           MSG_Y_COORDINATE, TEXT_SIZE_LARGE, YAY_MSG);
    levelLength++;
    break;
  case waiting_yay_st:
    yayCounter++;
    break;
  case start_new_level_st:
    break;
  case waiting_new_level_st:
    newLevelCounter++;
    break;
  case start_end_game_st:
    simonControl_printText(DISPLAY_WHITE, TRY_AGAIN_MSG_X_COORDINATE,
                           MSG_Y_COORDINATE, TEXT_SIZE_SMALL,
                           LAST_SEQUENCE_MSG);
    display_setCursor(SEQUENCE_MSG_X_COORDINATE, MSG_Y_COORDINATE);
    if (currentLevel == levelLength - 1) {
      currentLevel = levelLength;
    }
    char str1[FIRST_SEQUENCE];
    sprintf(str1,
            "%d"
            "%s"
            "%d",
            currentLevel - ONE_INIT, "/", levelLength);
    display_print(str1);
    break;
  case waiting_end_game_st:
    endCounter++;
    break;
  }

  // state transitions
  switch (currentStateSC) {
  case init_st:
    // Makes a new sequence if the current sequence was beat.
    if (newSequence) {
      generateSequence();
      globals_setSequence(longSequence, currentSequenceLength);
      currentStateSC = start_sc_st;
    } else if (display_isTouched()) {
      generateSequence();
      globals_setSequence(longSequence, currentSequenceLength);
      currentStateSC = waiting_untouch_st;
    }
    break;
  case waiting_untouch_st:
    // Waits until the display is no longer touched.
    if (!display_isTouched()) {
      simonControl_blackSquare();
      simonControl_printText(DISPLAY_BLACK, SIMON_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_LARGE, SIMON_MSG);
      simonControl_printText(DISPLAY_BLACK, TOUCH_TO_PLAY_MSG_X_COORDINATE,
                             TOUCH_TO_PLAY_MSG_Y_COORDINATE, TEXT_SIZE_SMALL,
                             TOUCH_TO_PLAY_MSG);
      display_setTextColor(DISPLAY_WHITE);
      currentStateSC = start_sc_st;
    }
    break;
  case start_sc_st:
    currentStateSC = enable_fs_st;
    break;
  case enable_fs_st:
    currentStateSC = waiting_fs_st;
    break;
  case waiting_fs_st:
    // Waits until flash sequence to finish.
    if (flashSequence_isComplete()) {
      currentStateSC = disable_fs_st;
    }
    break;
  case disable_fs_st:
    currentStateSC = enable_vs_st;
    break;
  case enable_vs_st:
    currentStateSC = waiting_vs_st;
    break;
  case waiting_vs_st:
    // Waits until verify sequence to finish.
    if (verifySequence_isComplete()) {
      currentStateSC = disable_vs_st;
    }
    break;
  case disable_vs_st:
    // If the sequence is over and complete.
    if (currentLevel == currentSequenceLength) {
      currentStateSC = start_yay_st;
    }
    // If there was a timeout or wrong move.
    else if (verifySequence_isTimeOutError() ||
             verifySequence_isUserInputError()) {
      wrongMove = ONE_INIT;
      currentStateSC = start_end_game_st;
    } else {
      currentLevel++;
      currentStateSC = start_sc_st;
    }
    break;
  case start_yay_st:
    currentStateSC = waiting_yay_st;
    break;
  case waiting_yay_st:
    // Waits for the yayCounter to finish.
    if (yayCounter == YAY_DURATION) {
      yayCounter = ZERO_INIT;
      simonControl_printText(DISPLAY_BLACK, YAY_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_LARGE, YAY_MSG);
      simonControl_printText(DISPLAY_WHITE, TRY_AGAIN_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_SMALL, TRY_AGAIN_MSG);
      currentStateSC = start_new_level_st;
    }
    break;
  case start_new_level_st:
    currentStateSC = waiting_new_level_st;
    break;
  case waiting_new_level_st:
    // Checks if the player touched the screen or not to try again.
    if (display_isTouched()) {
      simonControl_printText(DISPLAY_BLACK, TRY_AGAIN_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_SMALL, TRY_AGAIN_MSG);
      display_setTextColor(DISPLAY_WHITE);
      currentSequenceLength++;
      currentLevel = ONE_INIT;
      initDone = ZERO_INIT;
      yayCounter = ZERO_INIT;
      endCounter = ZERO_INIT;
      newLevelCounter = ZERO_INIT;
      newSequence = ONE_INIT;
      currentStateSC = init_st;
    } else if (newLevelCounter == YAY_DURATION) {
      simonControl_printText(DISPLAY_BLACK, TRY_AGAIN_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_SMALL, TRY_AGAIN_MSG);
      display_setTextColor(DISPLAY_WHITE);
      currentStateSC = start_end_game_st;
    }
    break;
  case start_end_game_st:
    currentStateSC = waiting_end_game_st;
    break;
  case waiting_end_game_st:
    // Waits until the yayCounter is done.
    if (endCounter == YAY_DURATION) {
      simonControl_printText(DISPLAY_BLACK, TRY_AGAIN_MSG_X_COORDINATE,
                             MSG_Y_COORDINATE, TEXT_SIZE_SMALL,
                             LAST_SEQUENCE_MSG);
      display_setCursor(SEQUENCE_MSG_X_COORDINATE, MSG_Y_COORDINATE);
      char str2[FIRST_SEQUENCE];
      sprintf(str2, "%d", currentSequenceLength);
      display_println(str2);
      display_setTextColor(DISPLAY_BLACK);
      display_println(str2);
      simonControl_blackSquare();
      simonControl_init();
      currentStateSC = init_st;
    }
    break;
  }
}
