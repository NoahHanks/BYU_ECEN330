#include "minimax.h"
#include <stdio.h>
#define MINIMAX_BOARD_SIZE (MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
#define TOP_LFT_ROW_COL 0
#define MID_ROW_COL 1
#define END_ROW_COL 2

minimax_move_t move;

// inputs the board state and the current player and calulates the best move to
// make which will most likely lead to a win. It checks all possible outcomes
// and chooses the best one. It returns the score from the best outcome.
// current_player_is_x == true means X, current_player_is_x == false means O
int minimax(minimax_board_t *board, bool current_player_is_x) {
  move.column = TOP_LFT_ROW_COL;
  move.row = TOP_LFT_ROW_COL;

  minimax_score_t score =
      minimax_computeBoardScore(board, !current_player_is_x);

  // Recursion base case, there has been a win or a draw.
  // If game is over, you need to evaluate board based upon previous
  // player/opponent.
  if (minimax_isGameOver(score)) {
    return score;
    // Otherwise, you need to recurse.
  }

  int index = 0;
  minimax_score_t scores[MINIMAX_BOARD_SIZE] = {};
  minimax_move_t moves[MINIMAX_BOARD_SIZE] = {};

  // This loop will generate all possible boards.
  for (int row = TOP_LFT_ROW_COL; row < MINIMAX_BOARD_ROWS; row++) {
    // goes through every column
    for (int column = TOP_LFT_ROW_COL; column < MINIMAX_BOARD_COLUMNS;
         column++) {
      // checks for empty spots on the board
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
        // Assign the square to 'X'or 'O'.
        if (current_player_is_x) {
          board->squares[row][column] = MINIMAX_X_SQUARE;
        } else {
          board->squares[row][column] = MINIMAX_O_SQUARE;
        }
        score = minimax(board, !current_player_is_x);
        scores[index] = score;
        moves[index].row = row;
        moves[index].column = column;
        // Undo the change to the board (return the square to empty) prior to
        // next iteration of for-loops.
        board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
        index++;
      }
    }
  }
  minimax_score_t tempScore = scores[TOP_LFT_ROW_COL];

  // goes through the table of scores and finds the highest if the player is X
  // or the lowest for the computer.
  if (current_player_is_x) {
    // go through all the scores in the scores array
    for (int i = 0; i < index; i++) {
      // finds the highest score and keeps updating score with the new highest
      // score.
      if (scores[i] >= tempScore) {
        tempScore = scores[i];
        score = scores[i];
        move.row = moves[i].row;
        move.column = moves[i].column;
      }
    }
  }
  // if the computer is player, find the minumum score.
  else {
    // go through the entire score array
    for (int i = 0; i < index; i++) {
      // if the current score is lower than tempScore.
      if (scores[i] <= tempScore) {
        tempScore = scores[i];
        score = scores[i];
        move.row = moves[i].row;
        move.column = moves[i].column;
      }
    }
  }
  return score;
}

// this function will determine the best place to play next, get input from the
// board, the current player, and the row and column.
void minimax_computeNextMove(minimax_board_t *board, bool current_player_is_x,
                             uint8_t *row, uint8_t *column) {
  minimax(board, current_player_is_x);
  *row = move.row;
  *column = move.column;
}

// this simply checks to see if the score such that the game, returns true if
// score is 10, -10 or 0 or false otherwise.
bool minimax_isGameOver(minimax_score_t score) {
  // if the gmae is over
  if (score == MINIMAX_X_WINNING_SCORE || score == MINIMAX_O_WINNING_SCORE ||
      score == MINIMAX_DRAW_SCORE) {
    return true;
  }
  return false;
}

// this function determines if the current board is a win, lose, draw by
// checking all winning cases. returns 10, -10 or 0 if the player won, lost, or
// ended in a draw.
int16_t minimax_computeBoardScore(minimax_board_t *board,
                                  bool current_player_is_x) {
  int currentPlayer;
  int winningScore;

  // if player is true
  if (current_player_is_x) {
    currentPlayer = MINIMAX_X_SQUARE;
    winningScore = MINIMAX_X_WINNING_SCORE;
  }
  // if player is false
  else {
    currentPlayer = MINIMAX_O_SQUARE;
    winningScore = MINIMAX_O_WINNING_SCORE;
  }

  // checks all the rows and columns to see if there are three in a row to be a
  // win.
  for (int i = TOP_LFT_ROW_COL; i < MINIMAX_BOARD_ROWS; i++) {
    if (board->squares[i][TOP_LFT_ROW_COL] == currentPlayer &&
        board->squares[i][MID_ROW_COL] == currentPlayer &&
        board->squares[i][END_ROW_COL] == currentPlayer) {
      return winningScore;
    }
    if (board->squares[TOP_LFT_ROW_COL][i] == currentPlayer &&
        board->squares[MID_ROW_COL][i] == currentPlayer &&
        board->squares[END_ROW_COL][i] == currentPlayer) {
      return winningScore;
    }
  }

  // this checks diagonally from top left to bottom right for 3 in a row
  if (board->squares[TOP_LFT_ROW_COL][TOP_LFT_ROW_COL] == currentPlayer &&
      board->squares[MID_ROW_COL][MID_ROW_COL] == currentPlayer &&
      board->squares[(END_ROW_COL)][(END_ROW_COL)] == currentPlayer) {
    return winningScore;
  }

  // this checks diagonally from bottom left to top right for 3 in a row
  if (board->squares[TOP_LFT_ROW_COL][(END_ROW_COL)] == currentPlayer &&
      board->squares[MID_ROW_COL][MID_ROW_COL] == currentPlayer &&
      board->squares[(END_ROW_COL)][TOP_LFT_ROW_COL] == currentPlayer) {
    return winningScore;
  }

  // if we haven't won and there are still empty squares, the game isn't over
  for (int column = TOP_LFT_ROW_COL; column < MINIMAX_BOARD_ROWS; column++) {
    for (int row = TOP_LFT_ROW_COL; row < MINIMAX_BOARD_COLUMNS; row++) {
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
        return MINIMAX_NOT_ENDGAME;
      }
    }
  }
  return MINIMAX_DRAW_SCORE;
}

// this function will set the board to all empty squares
void minimax_initBoard(minimax_board_t *board) {
  // goes through all the rows of the board
  for (int row = TOP_LFT_ROW_COL; row < MINIMAX_BOARD_ROWS; row++) {
    // goes through all the rcolumns of the board
    for (int column = TOP_LFT_ROW_COL; column < MINIMAX_BOARD_COLUMNS;
         column++) {
      board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
    }
  }
}
