/************************************************************************
 * minesweeper.c														*
 *																		*
 * Author(s): Isaac Smith
 *            Sam Essenburg
 ***********************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>




#define BOARD_SIZE_MIN 5
#define BOARD_SIZE_MAX 15
#define PCT_MINES_MIN 10
#define PCT_MINES_MAX 70


typedef struct {
	bool is_mine;
	int mines;
	bool visible;
}Cell;

typedef enum {WON, LOST, INPROGRESS} Status;

/************************************************************************
 * YOU MUST NOT DEFINE ANY GLOBAL VARIABLES (i.e., OUTSIDE FUNCTIONS).  *
 * COMMUNICATION BETWEEN FUNCTIONS MUST HAPPEN ONLY VIA PARAMETERS.     *                           *
 ************************************************************************/

/************************************************************************
 * Function declarations/prototypes										*
 ************************************************************************/
void displayMenu();

int getBoardSize();

int getPercentMines();

void initBoard(int size, Cell board[][size]);

void placeMinesOnBoard(int size, Cell board[][size], int nbrMines);

void fillInMineCountForNonMineCells(int size, Cell board[][size]);

int nbrOfMines(int size, Cell board[][size]);

int getNbrNeighborMines(int row, int col, int size, Cell board[][size]);

void displayBoard(int size, Cell board[][size], bool displayMines);

Status selectCell(int row, int col, int size, Cell board[][size]);

int nbrVisibleCells(int size, Cell board[][size]);

void setImmediateNeighborCellsVisible(int row, int col, int size, Cell board[][size]);

void setAllNeighborCellsVisible(int row, int col, int size, Cell board[][size]);


/************************************************************************
 * Main driver of the program. Uses the functions defined above.		*
 ************************************************************************/
int main(){
	int row, col, size, nbrMines;
	char command;
	bool displayMines = false;
	Status gameState = INPROGRESS;

	printf("!!!!!WELCOME TO THE MINESWEEPER GAME!!!!!\n\n");
	size = getBoardSize();
	

	// declare 2D array of cells
	Cell board[size][size];
	initBoard(size,board);

	// determine number of mine to place on the board
	nbrMines = (int) (size * size * (getPercentMines()/100.0));
	printf("Num of Mines: %d\n", nbrMines);

	// place mines randomly on the board
	placeMinesOnBoard(size,board,nbrMines);

	// For each non-mine cell, sets the neighboring mine count
	fillInMineCountForNonMineCells(size,board);

	displayBoard(size,board,displayMines);

	while (true) {
		printf("Enter command (m/M for command menu): ");
		scanf("%c",&command);
		getchar();

		switch (command) {
			case 'm': case 'M':
				displayMenu();
				break;

			case 'c': case 'C':
				do {
					printf("Enter row and column of cell: ");
					scanf("%d%d",&row,&col);
					getchar();
					if (row < 1 || row > size || col < 1 || col > size) {
						printf("Invalid row or column values. Try again.\n");
					}
				} while (row < 1 || row > size || col < 1 || col > size);
				row--;
				col--;
				gameState = selectCell(row,col,size,board);
				displayBoard(size,board,displayMines);
				break;

			case 's': case 'S':
				displayMines = true;
				displayBoard(size,board,displayMines);
				break;

			case 'h': case 'H':
				displayMines = false;
				displayBoard(size,board,displayMines);
				break;

			case 'b': case 'B':
				displayBoard(size,board,displayMines);
				break;

			case 'q': case 'Q':
				printf("Bye.\n");
				return 0;

			default:
				printf("Invalid command. Try again.\n");
		}

		// display appropriate message if the game is over
		if (gameState == WON) {
			printf("You found all the mines. Congratulations. Bye.");
			return 0;
		} else if (gameState == LOST) {
			printf("Oops. Sorry, you landed on a mine. Bye");
			return 0;
		}
	}

	return 0;
}

/************************************************************************
 * Displays list of commands allowed at the prompt						*
 ************************************************************************/
void displayMenu()
{
  printf("List of available commands:\n");
  printf("   Show Mines: s/S\n");
  printf("   Hide Mines: h/H\n");
  printf("   Select Cell: c/C\n");
  printf("   Display Board: b/B\n");
  printf("   Display Menu: m/M\n");
  printf("   Quit: q/Q\n\n");
}

/************************************************************************
 * Initializes the fields of each cell on the board as follows:			*
 * 		is_mine field to false											*
 * 		mines field to 0												*
 * 		visible field to false											*
 ************************************************************************/
void initBoard(int size, Cell board[][size])
{
  // TO DO

  for(int row = 0; row < size; row++){
    for(int col = 0; col < size; col++){
      board[row][col].is_mine = false;
      board[row][col].visible = false;
      board[row][col].mines = 0;
    }
  }
	
}

/************************************************************************
 * Places the specified number of mines randomly on the board			*
 ************************************************************************/
void placeMinesOnBoard(int size, Cell board[][size], int nbrMines)
{
  // TO DO

  int i = 0;

  //printf("%d\n", nbrMines);
  while(i < nbrMines){
    srand(time(NULL));
    int row = rand()%size;
    int col = rand()%size;
    if(board[row][col].is_mine == false){
      board[row][col].is_mine = true;
      i = i + 1;
    }else{
      board[row][col].is_mine = true;
    }
  }
}

/************************************************************************
 * For each non-mine cell on the board, set the "mines" field to the	*
 * number of mines in the immediate neighborhood.						*
 ************************************************************************/
void fillInMineCountForNonMineCells(int size, Cell board[][size])
{
  // TO DO

  for(int row = 0; row < size; row++){
    for(int col = 0; col < size; col++){
      if(board[row][col].is_mine == false){
 //       for(int i = row-1; i < row + 2; i++){
  //        for(int j = col-1; j < col + 2; j++){
  //          if(board[i][j].is_mine == true && i < size && j < size)
  //            board[row][col].mines++;
            board[row][col].mines = getNbrNeighborMines(row, col, size, board);
          }
        }	
      }
    }
 

/************************************************************************
 * Counts and returns the number of mines on the board					*
 ************************************************************************/
int nbrOfMines(int size, Cell board[][size])
{
  int count = 0;

  for(int row = 0; row < size; row++){
    for(int col = 0; col < size; col++){
      if(board[row][col].is_mine == true)
        count++; 
    }
  }  

  return count;
}

/************************************************************************
 * Returns the number of mines in the immediate neighborhood of a cell	*
 * at location (row,col) on the board.									*
 ************************************************************************/
int getNbrNeighborMines(int row, int col, int size, Cell board[][size])
{
  int count = 0;

  if(row == 0 && col == 0){
    if(board[row][col+1].is_mine)
      count++;
    if(board[row+1][col].is_mine)
      count++;
    if(board[row+1][col+1].is_mine)
      count++;
  }
  else if(col == 0){
    for(int i = row -1; i <  row + 2;i++){
      for(int j = col; j < col + 2;j++){
        if(board[i][j].is_mine == true){
          count++;
        }
      }
    }
  }
  else if(row == 0){
    for(int i = row; i < row + 2; i++){
      for(int j = col - 1; j < col + 2; j++ ){
        if(board[i][j].is_mine == true)
          count++;
      }
    }
  }
  else{
  for(int i = row - 1; i < row + 2; i++){
    for(int j = col - 1; j < col + 2; j++){
      if(board[i][j].is_mine == true && i < size && j < size)
        count++;
    }
  }  
  }

  return count;
}

/************************************************************************
 * Displays the board. If a cell is not currently visible and has a		*
 * mine, show the mine if the displayMines is true. Used for debugging	*
 * and testing purposes.												*
 ************************************************************************/
void displayBoard(int size, Cell board[][size], bool displayMines)
{
  // TO DO
  printf(" ");

  for(int col = 0; col < size; col++){
    if(col < 9)
      printf("  %d", col+1);
    else
      printf(" %d", col+1);
  }
  printf("\n");
  
  for(int row = 0; row < size; row++){
    if(row < 9){
        printf("%d  ", row + 1);  
    }
    else
        printf("%d ", row + 1);
    for(int c = 0; c < size; c++){
      if(displayMines == true && board[row][c].is_mine){
        printf("*  ");
      }
      else if(board[row][c].visible == true){
        printf("%d  ", board[row][c].mines);
      }
      else{
        printf("?  ");
      }
    }
    printf("\n");
  }
	
	printf(" \n");
}

/************************************************************************
 * Prompts the user for board size, reads and validates the input		*
 * entered, and returns the integer if it is within valid range.		*
 * repeats this in a loop until the user enters a valid value.			*
 ************************************************************************/
int getBoardSize()
{
  int size = 0;
  printf("Please enter a board size. Must be >=5 and <=15\n\n");
  scanf("%d",&size);
	
  if(size < BOARD_SIZE_MIN){
    printf("sorry please enter a size >= 5\n\n");
    getBoardSize();
  }else if(size > BOARD_SIZE_MAX){
    printf("Sorry, please enter a size <= 15\n\n");
    getBoardSize();
  }
  
  return size;

}

/************************************************************************
 * Prompts the user for percentage of mines to place on the board,		*
 * reads and validates the input entered, and returns the integer if it	*
 * is within valid range. repeats this in a loop until the user enters	*
 * a valid value for board size.										*
 ************************************************************************/
int getPercentMines()
{
  int percent = 0;

  int valid = 0;
  // TO DO
  while(valid != 1){
    printf("Please choose the percentage of mines on the board.\n\n");
    scanf("%d",&percent);
    
    if(percent <= 0 || percent >=100){
      printf("Sorry the percent must be between 1 and 99. Please try again.\n\n");
    }else{
      valid = 1;
    }
  }
  return percent;
}

/************************************************************************
 * Process cell selection by user during the game						*
 ************************************************************************/
Status selectCell(int row, int col, int size, Cell board[][size])
{
  // TO DO
  int vis;
  if(board[row][col].is_mine){
    printf("Oop. Sorry, you landed on a mine. Bye");
    
    return LOST;
  }
  else{
    board[row][col].visible = true;
    setAllNeighborCellsVisible(row, col, size, board);
    for(int r = 0; r < size; r++){
      for(int c = 0; c < size; c++){
        if(board[r][c].visible == true && board[r][c].is_mine != true){
          vis++;
        }
      }
    }
    if(vis == ((size*size)-nbrOfMines(size,board))){
      return WON;
    }
  }

  return INPROGRESS;
}

/************************************************************************
 * Returns the number of cells that are currently visible.				*
 ************************************************************************/
int nbrVisibleCells(int size, Cell board[][size])
{
  int count = 0;

  for(int row = 0; row < size; row++){
    for(int col = 0; col < size; col++){
      if(board[row][col].visible == true)
        count++;
    }
  }
  
  return count;
}

/************************************************************************
 * If the mine count of a cell at location (row,col) is zero, then make	*
 * the cells ONLY in the immediate neighborhood visible.				*
 ************************************************************************/
void setImmediateNeighborCellsVisible(int row, int col, int size, Cell board[][size])
{
  if(board[row][col].mines == 0){
    for(int i = row - 1; i < row + 2; i++){
      for(int j = col - 1; j < col + 2; j++){
        board[i][j].visible = true;
      }
    }
  }
}

/************************************************************************
 * If the mine count of a cell at location (row,col) is zero, then make	*
 * the cells in the immediate neighborhood visible and repeat this		*
 * process for each of the cells in this set of cells that have a mine	*
 * count of zero, and so on.											*
 ************************************************************************/
void setAllNeighborCellsVisible(int row, int col, int size, Cell board[][size])
{

  
  if(board[row][col].mines == 0){
     for(int i = row - 1; i < row + 2; i++){
      for(int j = col - 1; j < col + 2; j++){
       board[i][j].visible = true;
         if(board[i][j].mines == 0){
          for(int r = i - 1; r < i + 2; r++){
            for(int c = j - 1; c < j + 2; c++){
              if(r > 0 && c > 0 && r < size-1 && c < size-1){
                board[r][c].visible == true;
              }
            }
          }
        }
      }
    }
  }
}
