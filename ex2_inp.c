/********************************
* Student name: Noam Simon      *
* Student ID: 208388850         *
* Course Exercise Group: 04     *
* Exercise name: Exercise 2-inp *
********************************/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 4
#define COLS 4
#define CELLS_NUM 16
#define MAX_LENGTH 120
#define BOARD_LENGTH 256
#define EXIT_STRING "BYE BYE\n"
#define READ_FROM_STDIN_ERROR "failed reading from stdin. (ex2_inp)\n"
#define WRITE_TO_STDOUT_ERROR "failed writing to stdout.\n"
#define SIGACTION_ERROR "sigaction error.\n"
#define OPEN_FILE_ERROR "failed to open file for read.\n"
#define WIN_STRING "Congratulations!\n"
#define LOSE_STRING "Game Over!\n"

void printBoardGraphicFormat(int *pInt);

void initializeSignalsHandler();

void sigUsr1Handler(int signum, siginfo_t *info, void *ptr);

void sigIntHandler(int signum, siginfo_t *info, void *ptr);

void fromStringToMatrix(char* stringBroad, int board[CELLS_NUM]);

void printWinMessage();

/***************************************************************************
* function name : printToStdout                                            *
* input : string and a length of the string                                *
* output : void.                                                           *
* explanation : print the string ti stdout, and check for errors.          *
***************************************************************************/
void printToStdout(char *string) ;

/*******************************************************************************
* function name : main                                                         *
* input :                                                                      *
* output :                                                                     *
* explanation : print the board in graphic format when sigUser1 is sent.       *
*******************************************************************************/
int main(int argc, char* argv[]) {
    // initial the signal handlers.
    initializeSignalsHandler();

    //waiting for signals.
    while (1) {
        pause();
    }
}

/*******************************************************************************
* function name : fromStringToMatrix                                           *
* input : string that represent the line format of the board,                  *
*         array for insert the number                                          *
* output : void                                                                *
* explanation : convert the string to an array of the numbers.                 *
*******************************************************************************/
void fromStringToMatrix(char* stringBroad, int board[CELLS_NUM]){
    char *token;
    int i = 0;

    /* get the first token */
    token = strtok(stringBroad, ",");

    /* walk through other tokens */
    while( token != NULL  && i < CELLS_NUM)
    {
        board[i++] = atoi(token);
        token = strtok(NULL, ",");
    }
}

/**************************************************************************************
* function name : sigIntHandler                                                       *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the int signal.                                              *
**************************************************************************************/
void sigIntHandler(int signum, siginfo_t *info, void *ptr) {
    printToStdout(EXIT_STRING);
    exit(EXIT_SUCCESS);
}

/**************************************************************************************
* function name : sigUsr1Handler                                                      *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the user1 signal.                                            *
**************************************************************************************/
void sigUsr1Handler(int signum, siginfo_t *info, void *ptr) {
    char lineFormat[MAX_LENGTH];
    //read the game board as line format from stdin.
    if (read(STDIN_FILENO, lineFormat, MAX_LENGTH * sizeof(char)) == -1){
        write(STDERR_FILENO, READ_FROM_STDIN_ERROR, strlen(READ_FROM_STDIN_ERROR));
        exit(EXIT_FAILURE);
    }

    if (strcmp(lineFormat, WIN_STRING) == 0){
        printToStdout(WIN_STRING);
    } else if (strcmp(lineFormat, LOSE_STRING) == 0){
        printToStdout(LOSE_STRING);
    }
    //print the board in graphic format.
    int board[CELLS_NUM];
    fromStringToMatrix(lineFormat, board);
    printBoardGraphicFormat(board);
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
* function name : initializeSignalsHandler                                  *
* input :                                                                   *
* output : void.                                                            *
* explanation : initialize the SignalsHandler of the following signals      *
****************************************************************************/
void initializeSignalsHandler() {
    struct sigaction action;
    sigset_t blockMask;

    // Establish the signal handler.
    sigfillset (&blockMask);
    sigdelset(&blockMask, SIGUSR1);
    sigdelset(&blockMask, SIGINT);
    action.sa_mask = blockMask;
    action.sa_flags =  SA_SIGINFO;

    //attach the SIGUSR1 handler to the signal.
    action.sa_sigaction = sigUsr1Handler;
    if (sigaction (SIGUSR1, &action, NULL) != 0) {
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));
        exit(EXIT_FAILURE);
    }

    //attach the SIGINT handler to the signal.
    action.sa_sigaction = sigIntHandler;
    if (sigaction (SIGINT, &action, NULL) != 0) {
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));
        exit(EXIT_FAILURE);
    }
}

/****************************************************************************
* function name : printBoardGraphicFormat                                   *
* input : The board game in line format - array of the number in the board. *
* output : void.                                                            *
* explanation : print the board in the graphic format.                      *
****************************************************************************/
void printBoardGraphicFormat(int *board) {
    char graphicFormat[BOARD_LENGTH];
    char buffer[7];
    int i, j, num;
    buffer[0] = '\0';
    graphicFormat[0] = '\0';

    // for each row.
    for (i = 0; i < ROWS; i++) {
        strcat(graphicFormat, "|");
        //print the row.
        for (j = 0; j < COLS; j++){
            num = board[j + COLS * i];
            if (num == 0) { // case is a 0 print a blank cell.
                strcat(graphicFormat, "      |");
                continue;
            }
            sprintf(buffer, " %.04d |", num);
            strcat(graphicFormat, buffer);
        }
        strcat(graphicFormat, "\n");
    }
    strcat(graphicFormat, "\n\0");

    // print the board in the graphic format to STDOUT.
    printToStdout(graphicFormat);
}


//int board[16] = {0,2,4,128,4,0,12,0,4,2,2024,32,16,128,2,0};
//    printBoardGraphicFormat(board);
//    return 0;