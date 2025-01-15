//
// Author: Chun Wang Warrick Tsui
// UTorID: tsuichu2
// Student number: 1009907068
//

#include <sys/resource.h>
#include <sys/time.h>

#include "liblab8part2.h"
#include "stdbool.h"
#include "stdio.h"

void createBoard(char board[][26], int n) {
  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if ((row == col) && ((row == n / 2 - 1) || (row == n / 2))) {
        board[row][col] = 'W';
      } else if (((row - col == 1) && (row == n / 2)) ||
                 ((col - row == 1) && (row == n / 2 - 1))) {
        board[row][col] = 'B';
      } else {
        board[row][col] = 'U';
      }
    }
  }
}

void printBoard(char board[][26], int n) {
  printf("  ");
  for (int letter = 0; letter < n; letter++) {
    printf("%c", 'a' + letter);
  }
  printf("\n");
  for (int row = 0; row < n; row++) {
    printf("%c ", 'a' + row);
    for (int col = 0; col < n; col++) {
      if (board[row][col] != NULL) {
        printf("%c", board[row][col]);
      }
    }
    printf("\n");
  }
}

bool positionInBounds(int n, int row, int col) {
  if ((row >= 0 && row < n) && (col >= 0 && col < n)) {
    return true;
  }
  return false;
}

bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {
  // find first appearance of the same colour
  bool foundColour = false;
  if (!positionInBounds(n, row, col)) {
    return false;
  }

  // Check trivial
  if (deltaRow == 0 && deltaCol == 0) {
    return false;
  }
  // check adjacent pieces
  if (board[row + deltaRow][col + deltaCol] == 'U' ||
      board[row + deltaRow][col + deltaCol] == colour) {
    return false;
  }

  int rowCheck = row + deltaRow;
  int colCheck = col + deltaCol;

  while (positionInBounds(n, rowCheck, colCheck)) {
    if ((board[rowCheck][colCheck] == 'U')) {
      return false;
    } else if ((board[rowCheck][colCheck] == colour) && !foundColour) {
      return true;
    }
    rowCheck += deltaRow;
    colCheck += deltaCol;
  }
  return foundColour;
}

bool isLegalMove(const char board[][26], int n, int row, int col, char colour) {
  bool legal = false;

  if (!positionInBounds(n, row, col) || board[row][col] != 'U') {
    return false;
  }
  for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
      if (checkLegalInDirection(board, n, row, col, colour, deltaRow,
                                deltaCol)) {
        legal = true;
      }
    }
  }
  return legal;
}

void flipDir(char board[][26], int n, int row, int col, char colour,
             int deltaRow, int deltaCol) {
  while (board[row + deltaRow][col + deltaCol] != colour) {
    board[row + deltaRow][col + deltaCol] = colour;
    row += deltaRow;
    col += deltaCol;
  }
}

void flipBoard(char board[][26], int n, int row, int col, char colour) {
  for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
      if (checkLegalInDirection(board, n, row, col, colour, deltaRow,
                                deltaCol)) {
        flipDir(board, n, row, col, colour, deltaRow, deltaCol);
      }
    }
  }
}

bool colorCanMove(char board[][26], int n, char color) {
  bool canMove = false;

  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (isLegalMove(board, n, row, col, color)) {
        canMove = true;
      }
    }
  }

  return canMove;
}

int scoreInDir(char board[][26], int n, int row, int col, char colour,
               int deltaRow, int deltaCol) {
  int scoreInDirection = 0;
  if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)) {
    while (board[row + deltaRow][col + deltaCol] != colour) {
      scoreInDirection++;
      row += deltaRow;
      col += deltaCol;
    }
  }
  return scoreInDirection;
}

int countPieces(char board[][26], int n, char colour) {
  int pieces = 0;
  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (board[row][col] == colour) {
        pieces++;
      }
    }
  }
  return pieces;
}

int findAvailableMoves(const char board[][26], int n, char turn,
                       int rowsAvailable[], int colsAvailable[]) {
  int movesAvailable = 0;

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (isLegalMove(board, n, row, col, turn)) {
        rowsAvailable[movesAvailable] = row;
        colsAvailable[movesAvailable] = col;
        movesAvailable++;
      }
    }
  }

  return movesAvailable;
}

int countStablePieces(const char board[][26], int n, int row, int col,
                      char compTurn) {
  int stablePieces = 0;
  for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
      if (checkLegalInDirection(board, n, row, col, compTurn, deltaRow,
                                deltaCol)) {
        while (board[row + deltaRow][col + deltaCol] == compTurn) {
          stablePieces++;
          row += deltaRow;
          col += deltaCol;
        }
      }
    }
  }
  return stablePieces;
}

int evaluate(const char board[][26], int n, char compTurn, char oppTurn) {
  int heuristic = 0;

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (board[row][col] == compTurn) {
        heuristic++;
        if ((row == 7 || row == 0) && (col == 7 || col == 0)) {
          heuristic += 50;
          heuristic += 10 * countStablePieces(board, n, row, col, compTurn);
        }
      } else if (isLegalMove(board, n, row, col, compTurn)) {
        heuristic += 2;
      }

      else {
        if (board[row][col] == oppTurn) {
          heuristic--;
          if ((row == 7 || row == 0) && (col == 7 || col == 0)) {
            heuristic -= 50;
            heuristic -= 10 * countStablePieces(board, n, row, col, oppTurn);
          }
        } else if (isLegalMove(board, n, row, col, oppTurn)) {
          heuristic -= 2;
        }
      }
    }
  }

  return heuristic;
}

int minimaxNumber(const char board[][26], int n, char originalTurn,
                  char checkTurn, int iteration) {
  char nextTurn = '\0';
  char oppTurn = '\0';

  if (originalTurn == 'B') {
    oppTurn = 'W';
  } else {
    oppTurn = 'B';
  }

  if (checkTurn == 'B') {
    nextTurn = 'W';
  } else {
    nextTurn = 'B';
  }

  if (iteration == 5) {
    return evaluate(board, n, originalTurn, oppTurn);
  }

  int movesAvailable = 0;
  int rowsAvailable[60];
  int colsAvailable[60];

  movesAvailable =
      findAvailableMoves(board, n, checkTurn, rowsAvailable, colsAvailable);

  if (movesAvailable == 0) {
    return minimaxNumber(board, n, originalTurn, nextTurn, iteration + 1);
  } else {
    for (int i = 0; i < movesAvailable; i++) {
      char newBoard[26][26];
      memcpy(newBoard, board, sizeof(char) * 26 * 26);

      // Flip the new board
      flipBoard(newBoard, n, rowsAvailable[i], colsAvailable[i], checkTurn);
      newBoard[rowsAvailable[i]][colsAvailable[i]] = checkTurn;

      return minimaxNumber(board, n, originalTurn, nextTurn, iteration + 1);
    }
  }
}

int makeMove(const char board[][26], int n, char compTurn, int* compRow,
             int* compCol) {
  int movesAvailable = 0;
  int rowsAvailable[60];
  int colsAvailable[60];

  int bestRow = -1;
  int bestCol = -1;

  char oppTurn = '\0';
  if (compTurn == 'B') {
    oppTurn = 'W';
  } else {
    oppTurn = 'B';
  }

  movesAvailable =
      findAvailableMoves(board, n, compTurn, rowsAvailable, colsAvailable);

  int bestScore = -9999;

  for (int i = 0; i < movesAvailable; i++) {
    char newBoard[26][26];
    memcpy(newBoard, board, sizeof(char) * 26 * 26);
    newBoard[rowsAvailable[i]][colsAvailable[i]] = compTurn;
    flipBoard(newBoard, n, rowsAvailable[i], colsAvailable[i], compTurn);

    // Killer Moves: Corners or turn blockage
    if ((rowsAvailable[i] == 7 || rowsAvailable[i] == 0) &&
        (colsAvailable[i] == 7 || colsAvailable == 0)) {
      bestScore = 9999;
      bestRow = rowsAvailable[i];
      bestCol = colsAvailable[i];
    } else if (!colorCanMove(newBoard, n, oppTurn)) {
      bestScore = 9999;
      bestRow = rowsAvailable[i];
      bestCol = colsAvailable[i];
    } else {
      int currentScore = minimaxNumber(newBoard, n, compTurn, oppTurn, 1);
      if (currentScore > bestScore) {
        bestScore = currentScore;
        bestRow = rowsAvailable[i];
        bestCol = colsAvailable[i];
      }
    }
  }

  *compRow = bestRow;
  *compCol = bestCol;
  return 0;
}

int main(void) {
  // Board setup
  int size = 0;
  char board[26][26];

  // User setup
  char userColor = '\0';
  int userRow = 0;
  int userCol = 0;

  // Computer AI setup
  char compColor = '\0';
  bool compTurn = true;  // true = black
  bool continueGame = true;
  int compRow = 0;
  int compCol = 0;

  // Game Result setup
  int blacks = 0;
  int whites = 0;

  printf("Enter the board dimension: ");
  scanf("%d", &size);

  printf("Computer plays (B/W): ");
  scanf(" %c", &compColor);

  // Add validation for colors if needed
  if (compColor == 'W') {
    userColor = 'B';
  } else {
    userColor = 'W';
  }

  createBoard(board, size);
  printBoard(board, size);

  continueGame =
      colorCanMove(board, size, 'B') || colorCanMove(board, size, 'W');

  if (compColor == 'W') {
    compTurn = false;
  }

  while (continueGame) {
    // Computer's Turn
    if (compTurn) {
      if (colorCanMove(board, size, compColor)) {
        struct rusage
            usage;  // a structure to hold "resource usage" (including time)
        struct timeval start, end;  // will hold the start and end times
        getrusage(RUSAGE_SELF, &usage);
        start = usage.ru_utime;
        double timeStart =
            start.tv_sec + start.tv_usec / 1000000.0;  // in seconds

        makeMove(board, size, compColor, &compRow, &compCol);

        getrusage(RUSAGE_SELF, &usage);
        end = usage.ru_utime;
        double timeEnd = end.tv_sec + end.tv_usec / 1000000.0;  // in seconds
        double totalTime = timeEnd - timeStart;

        // printf("\nTime elapsed: %.5lf\n", totalTime);

        board[compRow][compCol] = compColor;
        flipBoard(board, size, compRow, compCol, compColor);
        printf("Computer places %c at %c%c.\n", compColor,
               (char)(compRow + 'a'), (char)(compCol + 'a'));
        printBoard(board, size);
      } else {
        printf("%c player has no valid move.\n", compColor);
      }
      compTurn = false;
    }

    // User's turn
    else {
      if (colorCanMove(board, size, userColor)) {
        findSmarterMove(board, size, userColor, &userRow, &userCol);
        flipBoard(board, size, userRow, userCol, userColor);
        board[userRow][userCol] = userColor;
        printBoard(board, size);

      } else {
        printf("%c player has no valid move.\n", userColor);
      }
      compTurn = true;
    }
    if (continueGame) {
      continueGame =
          colorCanMove(board, size, 'B') || colorCanMove(board, size, 'W');
    }
  }

  // DETERMING THE WINNER OF THE GAME
  blacks = countPieces(board, size, 'B');
  whites = countPieces(board, size, 'W');

  printf("Black: %d\n", blacks);
  printf("Whites: %d\n", whites);

  if (blacks > whites) {
    printf("B player wins.\n");
  } else if (whites > blacks) {
    printf("W player wins.\n");
  } else {
    printf("Draw!\n");
  }
  return 0;
}
