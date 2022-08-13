#include "./includes/sequence.h"
#include "./includes/sumo.h"

t_usrfunc usrfunc;

void signalHandler(int signum, void* data) {
    if (signum == 2) {
        t_sensors *sensors;
        sensors = (t_sensors *)data;
        freeNavigation();
        stopSensors(sensors);
        freeSumo(sensors, sensors->mot);
        if (usrfunc.fSig) {
            (*(usrfunc.fSig))();
        }
    }
    #ifdef DEBUG
        printf("SIG: %d\n", signum);
    #endif
    gpioTerminate();
    exit(signum);
}

/**
 * pass in NULL if you don't want to call a user function
 **/
int initSequence(void *edge, void *range, void *program, void* sigProgram, t_sensors *sensors) {

    if (sensors) {
        usrfunc.sensors = sensors;
        sensors->usrfn = &usrfunc;
        usrfunc.fEdge = edge;
        usrfunc.fRange = range;
        usrfunc.fPrgm = program;
        usrfunc.fSig = sigProgram;
        usrfunc.seqNext = 0;
        usrfunc.seqPrev = -1;
        usrfunc.seqExecuting = -1;
        usrfunc.seqRun = 1;
        usrfunc.eisr = 0;
        usrfunc.risr = 0;
        usrfunc.startTick = gpioTick();
    }
    else
        return EXIT_FAILURE;

    if (gpioSetSignalFuncEx(2, signalHandler, sensors)) {
        perror("Failed to set signal handler\n");
        syslog(LOG_DEBUG, "ERROR: gpioSetSignalFailure (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        printf("initSequence Finished\n");
    #endif
    return EXIT_SUCCESS;
}

/**
 * run Sequence, this is a blocking call
 * seconds = Delay Loop in seconds... ie 1.5 = 1 second + 500 miliseconds
 * returns 0 on completion of the calling user program
 * 	else returns 1 on failure
 **/
int runSequence(double seconds) {
    while(usrfunc.seqRun) {
        if (usrfunc.fPrgm) {
            (*(usrfunc.fPrgm))(usrfunc.sensors);
            time_sleep(seconds);
        }
        else {
            perror("Undefined User Program\n");
            syslog(LOG_DEBUG, "ERROR: sumoUndefUsrProg (%s,%d)", __FILE__, __LINE__);
            return EXIT_FAILURE;
        }
    }
    #ifdef DEBUG
        printf("Exiting Run Sequence\n");
    #endif
    return EXIT_SUCCESS;
}

/**
 * Stops the running sequence
 **/
int stopSequence(void) {
    usrfunc.seqRun = 0;
    return EXIT_SUCCESS;
}

/**
 * Retunrs 1 if running, else 0
 **/
int isSequenceRunning(void) {
    return usrfunc.seqRun;
}
