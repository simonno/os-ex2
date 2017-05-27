/********************************
* Student name: Noam Simon      *
* Student ID: 208388850         *
* Course Exercise Group: 04     *
* Exercise name: Exercise 2-inp *
********************************/

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define WIN_VAL 2024
#define ROWS 4
#define COLS 4
#define CELLS_NUM 16
#define MAX_LENGTH 100
#define BOARD_LENGTH 116
#define WIN_STRING "Congratulations!\n"
#define LOSE_STRING "Game Over!\n"
#define WRITE_TO_STDOUT_ERROR "failed writing to stdout.\n"
#define SIGACTION_ERROR "sigaction error.\n"
#define OUTPUT_FILE "output.txt"
#define DUP_ERROR "failed dup.\n"
#define OPEN_FILE_ERROR "failed to open file for read\n"


unsigned int rand_1_5();

void initializeBoard();

void printBoardLineFormat();

void run();

void addCellOf2();

void checkWinOrLose();

void printToStdout(char *string, size_t stringSize);

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
int oldSTDOUT;
int board[COLS][ROWS] = {0};

/*******************************************************************************
* function name : main                                                         *
* input :                                                                      *
* output :                                                                     *
* explanation :                                                                *
*******************************************************************************/
int main(int argc, char* argv[]) {
    if (argc < 2){
        exit(EXIT_FAILURE);
    }

    int fd = open(OUTPUT_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd < 0) {
        write(STDERR_FILENO, OPEN_FILE_ERROR, sizeof(OPEN_FILE_ERROR));
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDOUT_FILENO) < 0) {
        write(STDERR_FILENO, DUP_ERROR, sizeof(DUP_ERROR));
        exit(EXIT_FAILURE);
    }

    //save the old stdout.
    if ((oldSTDOUT = dup(STDOUT_FILENO)) < 0) {
        write(STDERR_FILENO, DUP_ERROR, sizeof(DUP_ERROR));
        exit(EXIT_FAILURE);
    }
    pidForSendingSig = atoi(argv[1]);
    initializeSignalsHandler();

    resetGame();

    run();
}

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

void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr) {
    addCellOf2();
    waitingVal = rand_1_5();
    alarm(waitingVal);
    printBoardLineFormat();
    // send a SIGUSR1 to the specific process.
    kill(pidForSendingSig, SIGUSR1);
}

void sigIntHandler(int sigNum, siginfo_t *info, void *ptr) {
    if (dup2(oldSTDOUT, STDOUT_FILENO) < 0) {
        write(STDERR_FILENO, DUP_ERROR, sizeof(DUP_ERROR));
        exit(EXIT_FAILURE);
    }
    close(oldSTDOUT);
    kill(pidForSendingSig, SIGINT);
    unlink(OUTPUT_FILE);
    exit(EXIT_SUCCESS);
}

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
}

void MoveRight() {
    int i, j, rightmost0, lastMarge;
    
    for (i = 0; i < ROWS; i++) {
        rightmost0  = COLS - 1;
        lastMarge = COLS - 1;
        for (j = COLS - 2 ; j >= 0; j--) {
            if (board[i][j] == board[i][j + 1]) {
                board[i][j + 1] *= 2;
                board[i][j] = 0;
                lastMarge = j;

            }
            if (board[i][rightmost0] != 0) {
                rightmost0--; // move the rightmost0  to the rightmost place 0 found.
                continue;
            }
            if (board[i][j] != 0) { // move cell right.
                board[i][rightmost0--] = board[i][j];
                board[i][j] = 0;
            }
        }
        // check if the is another cell to marge.
        if (board[i][lastMarge - 1] == board[i][lastMarge]) {
            board[i][lastMarge--] *= 2;
            if (board[i][lastMarge - 1] != 0){
                board[i][lastMarge--] = board[i][lastMarge - 1];
            }
            board[i][lastMarge] = 0;
        }
    }
}

void MoveDown() {
    int i, j, lower0, lastMarge;

    for (j = 0; j < COLS; j++) {
        lower0  = 0;
        lastMarge = 0;
        for (i = 1 ; i < ROWS; i--) {
            if (board[i][j] == board[i - 1][j]) {
                board[i - 1][j] *= 2;
                board[i][j] = 0;
                lastMarge = j;

            }
            if (board[lower0][j] != 0) {
                lower0++; // move the lower0 to the lower place 0 found.
                continue;
            }
            if (board[i][j] != 0) { // move cell right.
                board[lower0++][j] = board[i][j];
                board[i][j] = 0;
            }
        }
        // check if the is another cell to marge.
        if (board[lastMarge + 1][j] == board[lastMarge][j]) {
            board[lastMarge++][j] *= 2;
            if (board[lastMarge - 1][j] != 0){
                board[lastMarge++][j] = board[lastMarge + 1][j];
            }
            board[lastMarge][j] = 0;
        }
    }
}

void MoveUp() {
    int i, j, upper0, lastMarge;

    for (j = 0; j < COLS; j++) {
        upper0  = ROWS - 1;
        lastMarge = ROWS - 1;
        for (i = ROWS - 2 ; i >= 0; i--) {
            if (board[i][j] == board[i + 1][j]) {
                board[i + 1][j] *= 2;
                board[i][j] = 0;
                lastMarge = j;

            }
            if (board[upper0][j] != 0) {
                upper0--; // move the upper0 to the upper place 0 found.
                continue;
            }
            if (board[i][j] != 0) { // move cell right.
                board[upper0--][j] = board[i][j];
                board[i][j] = 0;
            }
        }
        // check if the is another cell to marge.
        if (board[lastMarge - 1][j] == board[lastMarge][j]) {
            board[lastMarge--][j] *= 2;
            if (board[lastMarge - 1][j] != 0){
                board[lastMarge--][j] = board[lastMarge - 1][j];
            }
            board[lastMarge][j] = 0;
        }
    }
}


void MoveLeft() {
    int i, j, leftmost0, lastMarge;

    for (i = 0; i < ROWS; i++) {
        leftmost0  = 0;
        lastMarge = 0;
        for (j = 1 ; j < COLS; j++) {
            if (board[i][j] == board[i][j - 1]) {
                board[i][j - 1] *= 2;
                board[i][j] = 0;
                lastMarge = j;

            }
            if (board[i][leftmost0] != 0) {
                leftmost0++; // move the leftmost0  to the leftmost place 0 found.
                continue;
            }
            if (board[i][j] != 0) { // move cell left.
                board[i][leftmost0++] = board[i][j];
                board[i][j] = 0;
            }
        }
        // check if the is another cell to marge.
        if (board[i][lastMarge + 1] == board[i][lastMarge]) {
            board[i][lastMarge++] *= 2;
            if (board[i][lastMarge + 1] != 0){
                board[i][lastMarge++] = board[i][lastMarge + 1];
            }
            board[i][lastMarge] = 0;
        }
    }
}
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
    
    checkWinOrLose();
}

/***************************************************************************
* function name : checkWinOrLose                                           *
* input :                                                                  *
* output : void.                                                           *
* explanation : check if the player win or lose the game -                 *
*               and print a message accordingly.                           *
***************************************************************************/
void checkWinOrLose() {
    int i, j;
    int numberOfEmptyCells = 16;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (board[i][j] == WIN_VAL) { // check if the player win.
                printToStdout(WIN_STRING, strlen(WIN_STRING)); // print win message.
                exit(EXIT_SUCCESS);
            } else if (board[i][j] != 0) { // count the number of empty cells.
                numberOfEmptyCells--;
            }
        }
    }
    // check if the board is full.
    if (numberOfEmptyCells == 0) {
        // the player lost.
        printToStdout(LOSE_STRING, strlen(LOSE_STRING)); // print win message.
        exit(EXIT_SUCCESS);
    }
}

/***************************************************************************
* function name : printToStdout                                            *
* input : string and a length of the string                                *
* output : void.                                                           *
* explanation : print the string ti stdout, and check for errors.          *
***************************************************************************/
void printToStdout(char *string, size_t stringSize) {
    if (write(STDOUT_FILENO, string, stringSize) < 0){ 
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
    printToStdout(lineFormat, strlen(lineFormat));
}

/************************************************************************************
* function name : initializeBoard                                                   *
* input :                                                                           *
* output : void.                                                                    *
* explanation : initialize the board to be 0 in each cell except two random cells.  *
************************************************************************************/
void initializeBoard() {
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