/********************************
* Student name: Noam Simon      *
* Student ID: 208388850         *
* Course Exercise Group: 04     *
* Exercise name: Exercise 2-inp *
********************************/

#include <stdio.h>
#include <signal.h>

#define ROWS 4
#define COLS 4
#define CELLS 16

void printBoard(int pInt[16]);

void initializeBlockSignals();

void sigUsr1Handler(int signum, siginfo_t *info, void *ptr);

void fromStringToMatrix(char* stringBroad, int board[CELLS]);

/*******************************************************************************
* function name : main                                                         *
* input :                                                    *
* output :                           *
* explanation :  *
*******************************************************************************/
int main(int argc, char* argv[]) {
    initializeBlockSignals();
    int board[CELLS] = {2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,0};
    printBoard(board);

    int board2[CELLS] = {2,4,0,0,2024,2,0,32,0,128,4,0,16,0,16,0};
    printBoard(board2);
}

void fromStringToMatrix(char* stringBroad, int board[CELLS]){

}

void sigUsr1Handler(int signum, siginfo_t *info, void *ptr)
{

}

void initializeBlockSignals() {
    struct sigaction usr_action;
    sigset_t block_mask;

    /* Establish the signal handler. */
    sigfillset (&block_mask);
    usr_action.sa_handler = (__sighandler_t) sigUsr1Handler;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    if (sigaction (SIGUSR1, &usr_action, NULL) != 0) {
        //error
    }

}

/****************************************************************************
* function name : printBoard                                                *
* input :The board game in line format - array of the number in the board.  *
* output : void.                                                            *
* explanation : print the board in the graphic format.                      *
****************************************************************************/
void printBoard(int board[CELLS]) {
    int i, j, num;

    // for each row.
    for (i = 0; i < ROWS; i++) {
        printf("|");
        //print the row.
        for (j = 0; j < COLS; j++){
            num = board[j + 4 * i];
            if (num == 0) { // case is a 0 print a blank cell.
                printf("      |");
                continue;
            }
            printf(" %.04d |", num);
        }
        printf("\n");
    }
    printf("\n");
}