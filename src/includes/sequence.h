#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "sumo.h"


/**
 * User Defined Functions
 * fEdge = called when any edge detector is triggered
 * fRange = called when any range detector is triggered
 * fPrgm = called once a second
 * seqNext = Next sequence to execute
 * seqExecuting = Current sequence executing
 * seqRun = 1 = run sequence, 0 = exit program
 * eisr = Edge 1 = ISR called, 0 = not called
 * risr = Range 1 = ISR called, 0 = not calles
 * startTick = start of init program tick;
 **/
typedef struct s_usrfunc {
    void (*fEdge)(t_sensors *, int, int);
    void (*fRange)(t_sensors *, int, int);
    void (*fPrgm)(t_sensors *);
    void (*fSig)(void);
    int seqNext;
    int seqPrev;
    int seqExecuting;
    unsigned seqRun;
    unsigned eisr;
    unsigned risr;
    uint32_t startTick;
    t_sensors *sensors;
}t_usrfunc;

int initSequence(void *edge, void *range, void* program, void* sigProgram, t_sensors *sensors);
int runSequence(double seconds);
int stopSequence(void);
int isSequenceRunning(void);

#endif
