#include "ticTacToeControl.h"
#include "ticTacToeDisplay.h"

#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "utils.h"
#include <stdio.h>

#define START_MESSAGE_EXPIRED 75
#define WAIT_TIMER_EXPIRED 25
#define ADC_IS_SETTLED 1
#define BUTTON_0_MASK 0x01

#define TOP_LFT_ROW_COL 0
#define MID_ROW_COL 1
#define END_ROW_COL 2

#define INVALID_MOVE -1
#define ZERO_INITIALIZER 0
#define ONE_INITIALIZER 1

#define TEXT_SIZE 2

#define X_THIRD (DISPLAY_WIDTH / 3)
#define X_SIXTH (DISPLAY_WIDTH / 6)
#define X_TWELFTH (DISPLAY_WIDTH / 12)

#define Y_THIRD (DISPLAY_HEIGHT / 3)
#define Y_SIXTH (DISPLAY_HEIGHT / 6)
#define Y_TWELFTH (DISPLAY_HEIGHT / 12)
#define Y_SIXTEENTH (DISPLAY_HEIGHT / 16)

#define WIDTH_TENTH DISPLAY_WIDTH / 10
#define HEIGHT_THIRD DISPLAY_HEIGHT / 3
#define MAX_UINT8 255

#define O_RADIUS 30
#define START_MESSAGE                                                          \
  "Touch board to play X\n         -or-\n  wait for the computer\n       and " \
  "play O."
#define INIT_ST_MSG "init state\n"
#define START_MESSAGE_ST_MSG "start_message_st\n"
#define DRAW_BOARD_ST_MSG "draw_board_st\n"
#define WAIT_FOR_TOUCH_ST_MSG "waiting for touch_st\n"
#define DISPLAY_UNTOUCHED_ST_MSG "display_untouched_st\n"
#define PLAYER_TURN_ST_MSG "player_turn_st\n"
#define ADC_SETTLE_ST_MSG "adc_settle_st\n"
#define COMPUTER_TURN_ST_MSG "computer_turn_st\n"
#define CHECK_SCORE_ST_MSG "check_score_st\n"
#define NEW_GAME_ST_MSG "new_game_st\n"
#define END_GAME_ST_MSG "end_game_st\n"

uint8_t displayInitialized = ZERO_INITIALIZER;
uint8_t boardInitialized = ZERO_INITIALIZER;
uint8_t startMessageCounter = ZERO_INITIALIZER;
uint8_t waitTimerCounter = ZERO_INITIALIZER;
uint8_t adcSettleCounter = ZERO_INITIALIZER;
uint8_t CPU_is_X = ZERO_INITIALIZER;
uint8_t playerJustMoved = ZERO_INITIALIZER;
uint8_t firstMove = ONE_INITIALIZER;
uint8_t move_row = MAX_UINT8;
uint8_t move_column = MAX_UINT8;
bool emptySquare = false;
minimax_move_t nextMove;
minimax_board_t boardControl;
minimax_board_t boardControlCopy;

// State machine states.
enum ticTacToe_st_t {
  init_st,          // Start here, stay in this state for just one tick.
  start_message_st, // Display the start message and instructions for 2 seconds
  draw_board_st,    // Draw the board. Wait for 4 seconds. If screen is touched,
                    // player goes first. Otherwise computer goes first.
  wait_for_touch_st,
  display_untouched_st,
  player_turn_st,   // Let the player make a move
  adc_settle_st,    // Let the ADC settle
  computer_turn_st, // Let the computer make a move
  check_score_st,
  new_game_st,
  end_game_st // Once the game is over, wait here till button 0 is pressed
} currentState = init_st;

// debugs the state machine by outputting the current state whenever the state
// changes.
void debugStatePrint() {
  static enum ticTacToe_st_t previousState;
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
    case start_message_st:
      printf(START_MESSAGE_ST_MSG);
      break;
    case draw_board_st:
      printf(DRAW_BOARD_ST_MSG);
      break;
    case wait_for_touch_st:
      printf(WAIT_FOR_TOUCH_ST_MSG);
      break;
    case display_untouched_st:
      printf(DISPLAY_UNTOUCHED_ST_MSG);
      break;
    case player_turn_st:
      printf(PLAYER_TURN_ST_MSG);
      break;
    case adc_settle_st:
      printf(ADC_SETTLE_ST_MSG);
      break;
    case computer_turn_st:
      printf(COMPUTER_TURN_ST_MSG);
      break;
    case check_score_st:
      printf(CHECK_SCORE_ST_MSG);
      break;
    case new_game_st:
      printf(NEW_GAME_ST_MSG);
      break;
    case end_game_st:
      printf(END_GAME_ST_MSG);
      break;
    }
  }
}

// Initialize the tic-tac-toe conroller state machine
void ticTacToeControl_init() {
  debugStatePrint();
  // checks to see if the display is already initialized.
  if (!displayInitialized) {
    display_setCursor(WIDTH_TENTH, HEIGHT_THIRD);
    display_setTextSize(TEXT_SIZE);
    display_setTextColor(DISPLAY_WHITE);
    display_println(START_MESSAGE);
  }
}

// erases the start message by rewriting it with black.
void eraseStartMessage() {
  // checks to see if the display is already initialized.
  if (!displayInitialized) {
    display_setCursor(WIDTH_TENTH, HEIGHT_THIRD);
    display_setTextSize(TEXT_SIZE);
    display_setTextColor(DISPLAY_BLACK);
    display_println(START_MESSAGE);
    displayInitialized = ONE_INITIALIZER;
  }
}

// erases the board by drawing back X's and O's in all nine squares.
void eraseBoard() {
  for (int r = TOP_LFT_ROW_COL; r < MINIMAX_BOARD_ROWS; r++) {
    for (int c = TOP_LFT_ROW_COL; c < MINIMAX_BOARD_COLUMNS; c++) {
      ticTacToeDisplay_drawX(r, c, true);
      ticTacToeDisplay_drawO(r, c, true);
    }
  }
}

// initlaizes the board by setting everything to empty.
void initializeBoard() {
  // checks to see if the board is already initialized.
  if (!boardInitialized) {
    for (int i = TOP_LFT_ROW_COL; i < MINIMAX_BOARD_ROWS; i++) {
      for (int j = TOP_LFT_ROW_COL; j < MINIMAX_BOARD_COLUMNS; j++) {
        boardControl.squares[i][j] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }
  boardInitialized = ONE_INITIALIZER;
}

// checks if a square on the board is empty.
void square_is_empty() {
  // checks the square to see if it's empty or not.
  if (boardControl.squares[move_row][move_column] != MINIMAX_EMPTY_SQUARE) {
    emptySquare = false;
  } else {
    emptySquare = true;
  }
}

// Tick the tic-tac-toe conroller state machine
void ticTacToeControl_tick() {
  // State actions
  ticTacToeControl_init();
  debugStatePrint();
  switch (currentState) {
  case init_st:
    initializeBoard();
    break;
  case start_message_st:
    startMessageCounter++;
    break;
  case draw_board_st:
    eraseStartMessage();
    ticTacToeDisplay_drawBoardLines();
    break;
  case display_untouched_st:
    break;
  case player_turn_st:
    ticTacToeDisplay_touchScreenComputeBoardRowColumn(&move_row, &move_column);
    square_is_empty();
    // checks if the square is empty and breaks if not empty.
    if (!emptySquare) {
      break;
    }
    // draws the correct shape arrording to who they're playing as.
    if (CPU_is_X) {
      ticTacToeDisplay_drawO(move_row, move_column, false);
      boardControl.squares[move_row][move_column] = MINIMAX_O_SQUARE;
    } else {
      ticTacToeDisplay_drawX(move_row, move_column, false);
      boardControl.squares[move_row][move_column] = MINIMAX_X_SQUARE;
    }
    playerJustMoved = ONE_INITIALIZER;
    break;
  case adc_settle_st:
    adcSettleCounter++;
    break;
  case computer_turn_st:
    minimax_computeNextMove(&boardControl, CPU_is_X, &move_row, &move_column);
    // draws an X if the computer is X or draws O otherwise.
    if (CPU_is_X) {
      ticTacToeDisplay_drawX(move_row, move_column, false);
      boardControl.squares[move_row][move_column] = MINIMAX_X_SQUARE;
    } else {
      ticTacToeDisplay_drawO(move_row, move_column, false);
      boardControl.squares[move_row][move_column] = MINIMAX_O_SQUARE;
    }
    playerJustMoved = ZERO_INITIALIZER;
    break;
  case end_game_st:
    break;
  case wait_for_touch_st:
    waitTimerCounter++;
    break;
  case check_score_st:
    break;
  case new_game_st:
    initializeBoard();
    eraseBoard();
    break;
  default:
    printf("clockControl_tick state action: hit default\n\r");
    break;
  }

  // State updates.
  switch (currentState) {
  case init_st:
    currentState = start_message_st;
    break;
  case start_message_st:
    // checks to see if the start timer is done before going to draw board.
    if (startMessageCounter == START_MESSAGE_EXPIRED) {
      startMessageCounter = ZERO_INITIALIZER;
      currentState = draw_board_st;
    }
    break;
  case draw_board_st:
    currentState = wait_for_touch_st;
    break;
  case display_untouched_st:
    // checks to see if the display is no longer touched.
    if (!display_isTouched()) {
      currentState = adc_settle_st;
    }
    break;
  case player_turn_st:
    move_row = INVALID_MOVE;
    move_column = INVALID_MOVE;
    // checks to see if the square pressed is empty.
    if (!emptySquare) {
      break;
    } else {
      currentState = check_score_st;
      emptySquare = false;
      break;
    }
  case adc_settle_st:
    // checks to see if the adc settle counter is finished.
    if (adcSettleCounter == ADC_IS_SETTLED) {
      adcSettleCounter = ZERO_INITIALIZER;
      currentState = player_turn_st;
    }
    break;
  case computer_turn_st:
    move_row = TOP_LFT_ROW_COL;
    move_column = TOP_LFT_ROW_COL;
    currentState = check_score_st;
    break;
  case end_game_st:
    // checks to see if just the last button is pressed.
    if (buttons_read() & BUTTON_0_MASK) {
      boardInitialized = ZERO_INITIALIZER;
      currentState = new_game_st;
    }
    break;
  case wait_for_touch_st:
    // checks to see if the wait timer is done and it's the player's turn.
    if (waitTimerCounter == WAIT_TIMER_EXPIRED && firstMove) {
      waitTimerCounter = ZERO_INITIALIZER;
      CPU_is_X = ONE_INITIALIZER;
      firstMove = ZERO_INITIALIZER;
      currentState = computer_turn_st;
    } else if (display_isTouched()) {
      waitTimerCounter = ZERO_INITIALIZER;
      // still checks if it's the player's turn before switching it to the
      // computer.
      if (firstMove) {
        CPU_is_X = ZERO_INITIALIZER;
        firstMove = ZERO_INITIALIZER;
      }
      display_clearOldTouchData();
      currentState = display_untouched_st;
    }
    break;
  case check_score_st:
    boardControlCopy = boardControl;
    // checks to see if the game cannot still be continued.
    if (minimax_computeBoardScore(&boardControlCopy, CPU_is_X) !=
            MINIMAX_NOT_ENDGAME ||
        minimax_computeBoardScore(&boardControlCopy, !CPU_is_X) !=
            MINIMAX_NOT_ENDGAME) {
      currentState = end_game_st;
    } else if (playerJustMoved) {
      currentState = computer_turn_st;
    } else {
      currentState = wait_for_touch_st;
    }
    break;
  case new_game_st:
    eraseBoard();
    firstMove = ONE_INITIALIZER;
    initializeBoard();
    currentState = draw_board_st;
    break;
  default:
    printf("clockControl_tick state update: hit default\n\r");
    break;
  }
}
