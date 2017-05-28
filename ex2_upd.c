/********************************
* Student name: Noam Simon      *
* Student ID: 208388850         *
* Course Exercise Group: 04     *
* Exercise name: Exercise 2-upd *
********************************/

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define WIN_VAL 2024
#define ROWS 4
#define COLS 4
#define CELLS_NUM 16
#define MAX_LENGTH 100
#define BOARD_LENGTH 116
#define WIN_STRING "Congratulations!\n"
#define LOSE_STRING "Game Over!\n"
#define SIGACTION_ERROR "sigaction error.\n"
#define WRITE_TO_STDOUT_ERROR "failed writing to stdout.\n"
#define NO_PID_ARG "pid process is missing."


unsigned int rand_1_5();

void initializeBoard();

void printBoardLineFormat();

void run();

void addCellOf2();

int checkWinOrLose();

void printToStdout(char *string);

void initializeSignalsHandler();

void sigIntHandler(int sigNum, siginfo_t *info, void *ptr);

void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr);

void sigUsr1Handler(int sigNum, siginfo_t *info, void *ptr);

void resetGame();

void MoveRight();

void MoveDown() ;

void MoveUp() ;

void MoveLeft() ;

int pidForSendingSig;
unsigned int waitingVal;
char moveDirection;
int board[COLS][ROWS];
//{0,2,4,128,4,0,0,0,4,2,2,32,16,16,0,0};

/*******************************************************************************
* function name : main                                                         *
* input :  a ip process of the specific process to send him a user1 sig.       *
* output :                                                                     *
* explanation :    run the game.                                               *
*******************************************************************************/
int main(int argc, char* argv[]) {
//    printBoardLineFormat();
//    //MoveRight();
//    //MoveLeft();
//    MoveDown();
//    printBoardLineFormat();
//    return 0;

    if (argc < 2) {
        write(STDERR_FILENO, NO_PID_ARG, sizeof(NO_PID_ARG));
        exit(EXIT_FAILURE);
    }

    pidForSendingSig = atoi(argv[1]);
    initializeSignalsHandler();

    resetGame();

    run();
}

/**************************************************************************************
* function name : run                                                                 *
* input :                                                                             *
* output :                                                                            *
* explanation :  run the game, get a input from the user that represent the movement. *
**************************************************************************************/
void run() {
    while (1) {
        // get a move direction which represent by a char.
        system("stty cbreak -echo");
        moveDirection = getchar();
        system("stty cooked echo");
        
        // send a SIGUSR1 to this process - for processing the movement.
        kill(getpid(), SIGUSR1);
    }
}

/**************************************************************************************
* function name : sigAlarmHandler                                                     *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the alarm signal.                                            *
**************************************************************************************/
void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr) {
    addCellOf2();
    waitingVal = rand_1_5();
    alarm(waitingVal);
    printBoardLineFormat();
    // send a SIGUSR1 to the specific process.
    kill(pidForSendingSig, SIGUSR1);

    //check if the player win or lose the game - and print a message accordingly.
    int checkVal = checkWinOrLose();
    if (checkVal == 1) {
        printToStdout(WIN_STRING); // print win message.
        kill(pidForSendingSig, SIGUSR1);
        exit(EXIT_SUCCESS);
    } else if (checkVal == -1) {
        printToStdout(LOSE_STRING); // print lose message.
        kill(pidForSendingSig, SIGUSR1);
        exit(EXIT_SUCCESS);
    }
}

/**************************************************************************************
* function name : sigIntHandler                                                       *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the int signal.                                              *
**************************************************************************************/
void sigIntHandler(int sigNum, siginfo_t *info, void *ptr) {
    exit(EXIT_SUCCESS);
}

/**************************************************************************************
* function name : sigUsr1Handler                                                      *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the user1 signal.                                            *
**************************************************************************************/
void sigUsr1Handler(int sigNum, siginfo_t *info, void *ptr) {
    //check the move char and update with the appropriate or create new game.
    switch (moveDirection) {
        case 'A':
            MoveLeft();
            break;
        case 'D':
            MoveRight();
            break;
        case 'W':
            MoveUp();
            break;
        case 'X':
            MoveDown();
            break;
        case 'S':
            resetGame();
            break;
        default:
            break;
    }

    //check if the player win or lose the game - and print a message accordingly.
    int checkVal = checkWinOrLose();
    if (checkVal == 1) {
        printToStdout(WIN_STRING); // print win message.
        kill(pidForSendingSig, SIGUSR1);
        exit(EXIT_SUCCESS);
    } else if (checkVal == -1) {
        printToStdout(LOSE_STRING); // print lose message.
        kill(pidForSendingSig, SIGUSR1);
        exit(EXIT_SUCCESS);
    }
}

/**************************************************************************************
* function name : MoveRight                                                           *
* input :                                                                             *
* output :                                                                            *
* explanation :  move the board right.                                                *
**************************************************************************************/
void MoveRight() {
    int squeesedRow[COLS];
    int i, j, t;

    for (i = 0; i < ROWS; i++) {
        t  = COLS - 1;
        memset(squeesedRow, 0, COLS * sizeof(int));
        for (j = COLS - 1; j >= 0; j--) { //squeese non-zero elements together to the right
            if(board[i][j] != 0) {
                squeesedRow[t--] = board[i][j];
            }
        }

        t  = COLS - 1;
        for (j = COLS - 1; j >= 0; j--) { // marge right.
            if (squeesedRow[j] == 0) { // after the first cell which is 0 all the cells in squeesedRow will be 0;
                break;
            }
            if(j > 0 && squeesedRow[j] == squeesedRow[j - 1]) { // case of marge
                board[i][t--] = 2 * squeesedRow[j--];
            } else {
                board[i][t--] = squeesedRow[j];
            }
        }

        while(t >= 0) { // set the rest cell as 0;
            board[i][t--] = 0;
        }
    }
}

/**************************************************************************************
* function name : MoveDown                                                            *
* input :                                                                             *
* output :                                                                            *
* explanation :  move the board down.                                                 *
**************************************************************************************/
void MoveDown() {
    int squeesedRow[ROWS];
    int i, j, t;

    for (i = 0; i < COLS; i++) {
        t  = ROWS - 1;
        memset(squeesedRow, 0, ROWS * sizeof(int));
        for (j = ROWS - 1; j >= 0; j--) { //squeese non-zero elements together upward.
            if(board[j][i] != 0) {
                squeesedRow[t--] = board[j][i];
            }
        }

        t = ROWS - 1;
        for (j = ROWS - 1; j >= 0; j--) { // marge right.
            if (squeesedRow[j] == 0) { // after the first cell which is 0 all the cells in squeesedRow will be 0;
                break;
            }
            if(j > 0 && squeesedRow[j] == squeesedRow[j - 1]) { // case of marge
                board[t--][i] = 2 * squeesedRow[j--];
            } else {
                board[t--][i] = squeesedRow[j];
            }
        }

        while(t >= 0) { // set the rest cell as 0;
            board[t--][i] = 0;
        }
    }
}

/**************************************************************************************
* function name : MoveUp                                                              *
* input :                                                                             *
* output :                                                                            *
* explanation :  move the board up.                                                   *
**************************************************************************************/
void MoveUp() {
    int squeesedRow[ROWS];
    int i, j, t;

    for (i = 0; i < COLS; i++) {
        t  = 0;
        memset(squeesedRow, 0, ROWS * sizeof(int));
        for (j = 0; j < ROWS; j++) { //squeese non-zero elements together upward.
            if(board[j][i] != 0) {
                squeesedRow[t++] = board[j][i];
            }
        }

        t = 0;
        for (j = 0; j < ROWS; j++) { // marge right.
            if (squeesedRow[j] == 0) { // after the first cell which is 0 all the cells in squeesedRow will be 0;
                break;
            }
            if(j < ROWS - 1 && squeesedRow[j] == squeesedRow[j + 1]) { // case of marge
                board[t++][i] = 2 * squeesedRow[j++];
            } else {
                board[t++][i] = squeesedRow[j];
            }
        }

        while(t < ROWS) { // set the rest cell as 0;
            board[t++][i] = 0;
        }
    }
}

/**************************************************************************************
* function name : MoveLeft                                                            *
* input :                                                                             *
* output :                                                                            *
* explanation :  move the board left.                                                 *
**************************************************************************************/
void MoveLeft() {
    int squeesedRow[COLS];
    int i, j, t;

    for (i = 0; i < ROWS; i++) {
        t  = 0;
        memset(squeesedRow, 0, COLS * sizeof(int));
        for (j = 0; j < COLS; j++) { //squeese non-zero elements together to the left
            if(board[i][j] != 0) {
                squeesedRow[t++] = board[i][j];
            }
        }

        t = 0;
        for (j = 0; j < COLS; j++) { // marge right.
            if (squeesedRow[j] == 0) { // after the first cell which is 0 all the cells in squeesedRow will be 0;
                break;
            }
            if(j < COLS - 1 && squeesedRow[j] == squeesedRow[j + 1]) { // case of marge
                board[i][t++] = 2 * squeesedRow[j++];
            } else {
                board[i][t++] = squeesedRow[j];
            }
        }

        while(t < COLS) { // set the rest cell as 0;
            board[i][t++] = 0;
        }
    }
}

/**************************************************************************************
* function name : resetGame                                                           *
* input :                                                                             *
* output :                                                                            *
* explanation : resetGame - initial new board , print the board in a line format,     *
*               send a user1 sig to the specific process. and set new alarm.          *
**************************************************************************************/
void resetGame() {
    waitingVal = rand_1_5();
    initializeBoard();

    printBoardLineFormat();
    // send a SIGUSR1 to the specific process.
    kill(pidForSendingSig, SIGUSR1);
    alarm(waitingVal);
}

/***************************************************************************
* function name : addCell                                                 *
* input :                                                                  *
* output : void.                                                           *
* explanation : add random cells of '2' to an empty cell in the board      *
***************************************************************************/
void addCellOf2() {
    // set a random cell to be 2
    int i = rand() % 5;
    int j = rand() % 5;

    // set another random cell to be 2;
    while(board[i][j] != 0) {
        i = rand() % 5;
        j = rand() % 5;
    }
    board[i][j] = 2;
}

/***************************************************************************
* function name : checkWinOrLose                                           *
* input :                                                                  *
* output :  1 - for winning, -1 - for losing, 0 else.                      *
* explanation : check if the player win or lose the game.                  *
***************************************************************************/
int checkWinOrLose() {
    int i, j;
    int numberOfEmptyCells = 16;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (board[i][j] == WIN_VAL) {
                // the player win.
                return 1;
            } else if (board[i][j] != 0) { // count the number of empty cells.
                numberOfEmptyCells--;
            }
        }
    }
    // check if the board is full.
    if (numberOfEmptyCells == 0) {
        // the player lost.
        return -1;
    }
    return 0;
}

/***************************************************************************
* function name : printToStdout                                            *
* input : string to print                                                  *
* output : void.                                                           *
* explanation : print the string ti stdout, and check for errors.          *
***************************************************************************/
void printToStdout(char *string) {
    if (write(STDOUT_FILENO, string, strlen(string)) < 0){
        write(STDERR_FILENO, WRITE_TO_STDOUT_ERROR, strlen(WRITE_TO_STDOUT_ERROR));
        exit(EXIT_FAILURE);
    }
}

/****************************************************************************
* function name : printBoardLineFormat                                      *
* input :                                                                   *
* output : void.                                                            *
* explanation : print the board in the line format.                         *
****************************************************************************/
void printBoardLineFormat() {
    char lineFormat[MAX_LENGTH] = "";
    char buffer[5] = "";
    int i, j;


    // print the first row.
    sprintf(buffer, "%d",board[0][0]);
    strcat(lineFormat, buffer);
    for (j = 1; j < COLS; j++){
        sprintf(buffer, ",%d",board[0][j]);
        strcat(lineFormat, buffer);
    }

    // print the other row.
    for (i = 1; i < ROWS; i++) {
        for (j = 0; j < COLS; j++){
            sprintf(buffer, ",%d",board[i][j]);
            strcat(lineFormat, buffer);
        }
    }
    strcat(lineFormat, "\n");
    printToStdout(lineFormat);
}

/************************************************************************************
* function name : initializeBoard                                                   *
* input :                                                                           *
* output : void.                                                                    *
* explanation : initialize the board to be 0 in each cell except two random cells.  *
************************************************************************************/
void initializeBoard() {
    memset(board, 0, ROWS * COLS * sizeof(int));
    // set a random cell to be 2
    int i = rand() % 5;
    int j = rand() % 5;
    board[i][j] = 2;

    // set another random cell to be 2;
    while(board[i][j] == 2) {
         i = rand() % 5;
         j = rand() % 5;
    }
    board[i][j] = 2;
}

/****************************************************************************
* function name : initializeSignalsHandler                                  *
* input :                                                                   *
* output : void.                                                            *
* explanation : initialize the SignalsHandler of the following signals      *
****************************************************************************/
void initializeSignalsHandler() {
    //initialize the sigaction struct.
    struct sigaction action;
    //initialize the blockSet.
    sigset_t blockMask;
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGUSR1);
    sigdelset(&blockMask, SIGINT);
    sigdelset(&blockMask, SIGALRM);
    action.sa_mask = blockMask;
    action.sa_flags = SA_SIGINFO;

    //attach the SIGUSR1 handler to the signal.
    action.sa_sigaction = sigUsr1Handler;
    if (sigaction(SIGUSR1, &action, NULL) != 0)
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));

    //attach the SIGALRM handler to the signal.
    action.sa_sigaction = sigAlarmHandler;
    if (sigaction(SIGALRM, &action, NULL) != 0)
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));

    //attach the SIGINT handler to the signal.
    action.sa_sigaction = sigIntHandler;
    if (sigaction(SIGINT, &action, NULL) != 0)
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));
}

/*******************************************************************************
* function name : rand_1_5                                                     *
* input :                                                                      *
* output : random num between 1 - 5.                                           *
* explanation : use the rand() function.                                       *
*******************************************************************************/
unsigned int rand_1_5() {
    return (unsigned int) (rand() % 5 + 1);
}