/********************************
* Student name: Noam Simon      *
* Student ID: 208388850         *
* Course Exercise Group: 04     *
* Exercise name: Exercise 2     *
********************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define EXEC_ERROR "failed to exec. \n"
#define SIGACTION_ERROR "sigaction error.\n"
#define CONNECTION_FILE "connectionFile.txt"
#define DUP_ERROR "failed dup.\n"
#define FORK_ERROR "failed fork.\n"
#define OPEN_FILE_ERROR "failed to open file\n"

void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr);

void initializeSignalsHandler();

int terminationTime;

int fdConnection;

//the two child processes
pid_t firstProcessPid, secondProcessPid;

/*******************************************************************************
* function name : main                                                         *
* input : the terminationTime - the time the program should run.               *
* output :                                                                     *
* explanation : create 2 process of ex2_inp and ex2_upd, by fork and exec,     *
*               and communicate between them.                                  *
*******************************************************************************/
int main(int argc, char *argv[]) {
    terminationTime = atoi(argv[1]);

    //initial the signals handlers.
    initializeSignalsHandler();

    // create a file through which information will be transferred between the children processes.
    fdConnection = open(CONNECTION_FILE, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IXGRP);
    if (fdConnection < 0) {
        write(STDERR_FILENO, OPEN_FILE_ERROR, sizeof(OPEN_FILE_ERROR));
        exit(EXIT_FAILURE);
    }


    //create child process for executing ex2_inp.
    firstProcessPid = fork();
    if (firstProcessPid == 0) { // first child process.
        // open the file through which information will be transferred between the children processes.
        fdConnection = open(CONNECTION_FILE, O_RDONLY);
        if (fdConnection < 0) {
            write(STDERR_FILENO, OPEN_FILE_ERROR, sizeof(OPEN_FILE_ERROR));
            exit(EXIT_FAILURE);
        }

        // set the input to be read from this file.
        if (dup2(fdConnection, STDIN_FILENO) < 0) {
            write(STDERR_FILENO, DUP_ERROR, sizeof(DUP_ERROR));
            exit(EXIT_FAILURE);
        }

        //executing ex2inp process.
        execlp("./ex2_inp.out", "./ex2_inp.out", NULL);
        write(STDERR_FILENO, EXEC_ERROR, strlen(EXEC_ERROR));
        exit(EXIT_FAILURE);

    } else if (firstProcessPid > 0) { // the father process.
        //creating another process for ex2_upd.
        secondProcessPid = fork();
        if (secondProcessPid == 0) { // second child process.

            // open the file through which information will be transferred between the children processes.
            fdConnection = open(CONNECTION_FILE, O_WRONLY);
            if (fdConnection < 0) {
                write(STDERR_FILENO, OPEN_FILE_ERROR, sizeof(OPEN_FILE_ERROR));
                exit(EXIT_FAILURE);
            }

            // set the output to be printed to this file.
            if (dup2(fdConnection, STDOUT_FILENO) < 0) {
                write(STDERR_FILENO, DUP_ERROR, sizeof(DUP_ERROR));
                exit(EXIT_FAILURE);
            }

            //convert the ex2inp pid to string.
            char arg[16];
            sprintf(arg, "%d", firstProcessPid);
            //executing ex2_upd.
            execlp("./ex2_upd.out", "./ex2_upd.out", arg, NULL);
            write(STDERR_FILENO, EXEC_ERROR, strlen(EXEC_ERROR));
            exit(EXIT_FAILURE);

        } else if (secondProcessPid > 0) { // the father process.
            sleep(1);
            alarm(terminationTime);

            //the father process wait for the processes to finish and exit.
            pause();
            return 0;
        }

        // case the second fork failed.
        write(STDERR_FILENO, FORK_ERROR, sizeof(FORK_ERROR));
        exit(EXIT_FAILURE);
    }

    // case the first fork failed.
    write(STDERR_FILENO, FORK_ERROR, sizeof(FORK_ERROR));
    exit(EXIT_FAILURE);
}

/**************************************************************************************
* function name : sigAlarmHandler                                                     *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the alarm signal.                                            *
**************************************************************************************/
void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr) {
    close(fdConnection);
    unlink(CONNECTION_FILE);

    //send to SIGINT to the processes to cause them finish the program.
    kill(firstProcessPid, SIGINT);
    kill(secondProcessPid, SIGINT);
}

/****************************************************************************
* function name : initializeSignalsHandler                                  *
* input :                                                                   *
* output : void.                                                            *
* explanation : initialize the SignalsHandler of the following signals      *
****************************************************************************/
void initializeSignalsHandler() {
    struct sigaction sigAct;
    //initialize the blockSet.
    sigset_t blockMask;
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGALRM);
    sigAct.sa_mask = blockMask;
    sigAct.sa_flags = SA_SIGINFO;

    //attach the SIGALRM handler to the signal.
    sigAct.sa_sigaction = sigAlarmHandler;
    if (sigaction(SIGALRM, &sigAct, NULL) != 0) {
        write(STDERR_FILENO, SIGACTION_ERROR, strlen(SIGACTION_ERROR));
        exit(EXIT_FAILURE);
    }
}
