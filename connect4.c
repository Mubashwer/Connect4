/**
  * Name: Mubashwer Salman Khurshid
  * Username: mskh
  * StudentID: 601738
  *
  * Connect4 game
  */
 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

#include "connect4.h"


int human_play(c4_t board, int move, int print)
{
    if (do_move(board, move, YELLOW)!=1) {
        if(print) printf("Panic\n");
        return STATUS_ABNORMAL;
    }
    if(print) print_config(board);
    /* and did they win??? */
    if (winner_found(board) == YELLOW) {
        /* rats, the person beat us! */
        if(print) printf("Ok, you beat me, beginner's luck!\n");
        return STATUS_CLIENT_WON;
    }
    /* was that the last possible move? */
    if (!move_possible(board)) {
        /* yes, looks like it was */
        if(print) printf("An honourable draw\n");
        return STATUS_DRAW;
    }
    /* otherwise, the game goes on */
    return move;
}


int ai_play(c4_t board, int move, int print)
{
    /* pretend to be thinking hard */
    if(print) printf("Ok, let's see now....");
    sleep(1);
    /* then play the move */
    if(print) printf(" I play in column %d\n", move);
    if (do_move(board, move, RED)!=1) {
        if(print) printf("Panic\n");
        return STATUS_ABNORMAL;
    }
    if(print) print_config(board);
    /* and did we win??? */
    if (winner_found(board) == RED) {
        /* yes!!! */
        if(print) printf("I guess I have your measure!\n");
        return STATUS_AI_WON;
    }
    /* was that the last possible move? */
    if (!move_possible(board)) {
        /* yes, looks like it was */
        if(print) printf("An honourable draw\n");
        return STATUS_DRAW;
    }   
    return move; 
}


/* Initialise the playing array to empty cells */
void
init_empty(c4_t board, int print) {
    int r, c;
    for (r=HEIGHT-1; r>=0; r--) {
        for (c=0; c<WIDTH; c++) {
            board[r][c] = EMPTY;
        }
    }
    if(print) printf("Welcome to connect-4 \n\n");
}

/* Apply the specified move to the board
 */
int
do_move(c4_t board, int c, char colour) {
    int r=0;
    /* first, find the next empty slot in that column */
    while ((r<HEIGHT) && (board[r][c-1]!=EMPTY)) {
        r += 1;
    }
    if (r==HEIGHT) {
        /* no move is possible */
        return 0;
    }
    /* otherwise, do the assignment */
    board[r][c-1] = colour;
    return 1;
}

/* Remove the top token from the specified column c
 */
void
undo_move(c4_t board, int c) {
    int r=0;
    /* first, find the next empty slot in that column, but be
     * careful not to run over the top of the array
     */
    while ((r<HEIGHT) && board[r][c-1] != EMPTY) {
        r += 1;
    }
    /* then do the assignment, assuming that r>=1 */
    board[r-1][c-1] = EMPTY;
    return;
}

/* Check board to see if it is full or not */
int
move_possible(c4_t board) {
    int c;
    /* check that a move is possible */
    for (c=0; c<WIDTH; c++) {
        if (board[HEIGHT-1][c] == EMPTY) {
            /* this move is possible */
            return 1;
        }
    }
    /* if here and loop is finished, and no move possible */
    return 0;
}


/* Read the next column number, and check for legality 
 */
int
get_move(c4_t board) {
    int c;
    /* check that a move is possible */
    if (!move_possible(board)) {
        return EOF;
    }
    /* one is, so ask for user input */
    printf("Enter column number: ");
    if (scanf("%d", &c) != 1) {
        return EOF;
    }
    /* and keep asking until a valid move is entered */
    while ((c<=0) || (c>WIDTH) || (board[HEIGHT-1][c-1]!=EMPTY)) {
        printf("That move is not possible. ");
        printf("Enter column number: ");
        if (scanf("%d", &c) != 1) {
            return EOF;
        }
    }
    /* now have a valid move */
    return c;
}

/* Print out the current configuration of the board 
 */
void
print_config(c4_t board) {
    int r, c, i, j;
    /* lots of complicated detail in here, mostly this function
     * is an exercise in attending to detail and working out the
     * exact layout that is required.
     */
    printf("\n");
    /* print cells starting from the top, each cell is spread over
     * several rows
     */
    for (r=HEIGHT-1; r>=0; r--) {
        for (i=0; i<HGRID; i++) {
            printf("\t|");
            /* next two loops step across one row */
            for (c=0; c<WIDTH; c++) {
                for (j=0; j<WGRID; j++) {
                    printf("%c", board[r][c]);
                }
                printf("|");
            }
            printf("\n");
        }
    }
    /* now print the bottom line */
    printf("\t+");
    for (c=0; c<WIDTH; c++) {
        for (j=0; j<WGRID; j++) {
            printf("-");
        }
        printf("+");
    }
    printf("\n");
    /* and the bottom legend */
    printf("\t ");
    for (c=0; c<WIDTH; c++) {
        for (j=0; j<(WGRID-1)/2; j++) {
            printf(" ");
        }
        printf("%1d ", c+1);
        for (j=0; j<WGRID-1-(WGRID-1)/2; j++) {
            printf(" ");
        }
    }
    printf("\n\n");
}

/* Is there a winning position on the current board?
 */
char
winner_found(c4_t board) {
    int r, c;
    /* check exhaustively from every position on the board
     * to see if there is a winner starting at that position.
     * could probably short-circuit some of the computatioin,
     * but hey, the computer has plenty of time to do the tesing
     */
    for (r=0; r<HEIGHT; r++) {
        for (c=0; c<WIDTH; c++) {
            if ((board[r][c]!=EMPTY) && rowformed(board,r,c)) {
                return board[r][c];
            }
        }
    }
    /* ok, went right through all positions and if we are still
     * here then there isn't a solution to be found
     */
    return EMPTY;
}


/* Is there a row in any direction starting at [r][c]?
 */
int
rowformed(c4_t board, int r, int c) {
    return
        explore(board, r, c, +1,  0) ||
        explore(board, r, c, -1,  0) ||
        explore(board, r, c,  0, +1) ||
        explore(board, r, c,  0, -1) ||
        explore(board, r, c, -1, -1) ||
        explore(board, r, c, -1, +1) ||
        explore(board, r, c, +1, -1) ||
        explore(board, r, c, +1, +1);
}

/* Nitty-gritty detail of looking for a set of straight-line
 * items all the same colour. Need to be very careful not to step
 * over the edge of the array
 */
int
explore(c4_t board, int r_fix, int c_fix, int r_off, int c_off) {
    int r_lim, c_lim;
    int r, c, i;
    r_lim = r_fix + (STRAIGHT-1)*r_off;
    c_lim = c_fix + (STRAIGHT-1)*c_off;
    /* can we go in the specified direction?
     */
    if (r_lim<0 || r_lim>=HEIGHT || c_lim<0 || c_lim>=WIDTH) {
        /* no, not enough space */
        return 0;
    }
    /* can, so check the colours for all the same */
    for (i=1; i<STRAIGHT; i++) {
        r = r_fix + i*r_off;
        c = c_fix + i*c_off;
        if (board[r][c] != board[r_fix][c_fix]) {
            /* found one different, so cannotbe a row */
            return 0;
        }
    }
    /* by now, a straight row all the same colour has been found */
    return 1;
}

/* Try to find a good move for the specified colour
 */
int
suggest_move(c4_t board, char colour) {
    int c;
    /* look for a winning move for colour */
    for (c=0; c<WIDTH; c++) {
        /* temporarily move in column c... */
        if (do_move(board, c+1, colour)) {
            /* ... and check to see the outcome */
            if (winner_found(board) == colour) {
                /* it is good, so unassign and return c */
                undo_move(board, c+1);
                return c+1;
            } else {
                undo_move(board, c+1);
            }
        }
    }
    /* ok, no winning move, look for a blocking move */
    if (colour == RED) {
        colour = YELLOW;
    } else {
        colour = RED;
    }
    for (c=0; c<WIDTH; c++) {
        /* temporarily move in column c... */
        if (do_move(board, c+1, colour)) {
            /* ... and check to see the outcome */
            if (winner_found(board) == colour) {
                /* it is good, so unassign and return c */
                undo_move(board, c+1);
                return c+1;
            } else {
                undo_move(board, c+1);
            }
        }
    }
    /* no moves found? then pick at random... */
    c = rand()%WIDTH;
    while (board[HEIGHT-1][c]!=EMPTY) {
        c = rand()%WIDTH;
    }
    return c+1;
}
