/**
  * Name: Mubashwer Salman Khurshid
  * Username: mskh
  * StudentID: 601738
  *
  * Library for Connect4 game
  */
  
#ifndef _CONNECT_4_INCLUDED_
#define _CONNECT_4_INCLUDED_


/*****************************************************************************/
/* Constants and type definitions */
/*****************************************************************************/

/* number of columns in the game */
#define WIDTH       7

/* number of slots in each column */
#define HEIGHT      6

/* number in row required for victory */
#define STRAIGHT    4

/* sign that a cell is still empty */
#define EMPTY      ' '

/* the two colours used in the game */
#define RED        'R'
#define YELLOW     'Y'

/* horizontal size of each cell in the display grid */
#define WGRID       5

/* vertical size of each cell in the display grid */
#define HGRID       3

#define RSEED   876545678

 /* game statuses */
#define STATUS_ABNORMAL -1

#define STATUS_AI_WON -2

#define STATUS_CLIENT_WON -3

#define STATUS_DRAW -4

typedef char c4_t[HEIGHT][WIDTH];

/*****************************************************************************/
/* Prototypes for Connect4 functions */
/*****************************************************************************/

/* Prints the game board */
void print_config(c4_t);
/* Initializes the game */
void init_empty(c4_t, int);
/* Tries to make a move */
int do_move(c4_t, int, char);
/* Undoes a move just made */
void undo_move(c4_t, int);
/* Asks client to make a move */
int get_move(c4_t);
/* Checks if a move is possible or not. */
int move_possible(c4_t);
/* Checks if user/AI won or not */
char winner_found(c4_t);
/* Checks if a winning row has formed or not */
int rowformed(c4_t,  int r, int c);
/* Explores the board */
int explore(c4_t, int r_fix, int c_fix, int r_off, int c_off);
/* AI move */
int suggest_move(c4_t board, char colour);
/* User move controller */
int human_play(c4_t, int, int);
/* AI move controller */
int ai_play(c4_t, int, int);

#endif
