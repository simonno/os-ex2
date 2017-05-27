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

#define EXEC_ERROR "failed to exec"
#define SIGACTION_ERROR "ERROR: sigaction"

void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr);

void initializeSignalsHandler();

int terminationTime;

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

    //create child process for executing ex2inp.
    if ((firstProcessPid = fork()) == 0) {

        //executing ex2inp process.
        execlp("./ex2_inp.out", "./ex2_inp.out", NULL);
        write(STDERR_FILENO, EXEC_ERROR, strlen(EXEC_ERROR));
        exit(EXIT_FAILURE);

    } else if (firstProcessPid > 0) {
        //creating another process for ex2upd.
        if ((secondProcessPid = fork()) == 0) {
            //convert the ex2inp pid to string.
            char arg[16];
            sprintf(arg, "%d", firstProcessPid);
            //executing ex2upd.

            execlp("./ex2_upd.out", "./ex2_upd.out", arg, NULL);
            write(STDERR_FILENO, EXEC_ERROR, strlen(EXEC_ERROR));
            exit(EXIT_FAILURE);

        } else if (secondProcessPid > 0) {
            sleep(1);
            alarm(terminationTime);

            //the father process wait for the processes to finish and exit.
            pause();
        }
    }
    return 0;
}

/**************************************************************************************
* function name : sigAlarmHandler                                                     *
* input : signal number, information number, and a pointer.                           *
* output :                                                                            *
* explanation :  handler the alarm signal.                                            *
**************************************************************************************/
void sigAlarmHandler(int sigNum, siginfo_t *info, void *ptr) {
    //send to ex2upd SIGINT and ex2upd will send to ex2inp SIGINT.
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
