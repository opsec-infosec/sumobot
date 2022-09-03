#include "./includes/sequence.h"
#include "./includes/display.h"
#include "./includes/sumo.h"
//#include "./includes/watchdog.h"

// Signal Interrupt Cntrl-C
void *sigPgrm(void) {
    #ifdef DISPLAY
        freeDisplay();
    #endif
    #ifdef WDT
        stopWDT();
    #endif
    return NULL;
}

// Edge Detector Active Low
void edge(t_sensors *sensors, int level, int gpio) {
    if (!level && !sensors->usrfn->eisr) {
        sensors->usrfn->eisr = 1;
        sensors->usrfn->seqPrev = sensors->usrfn->seqNext;
        if (gpio == sensors->p_edge[LEFT]) {
            sensors->usrfn->seqNext = 10;
        }
        else if (gpio == sensors->p_edge[RIGHT]) {
            sensors->usrfn->seqNext = 12;
        }
        else {
            sensors->usrfn->seqNext = 14;
        }
    }
}

// Range Detector Active Low
// Edge Detector takes Presidence
void range(t_sensors *sensors, int level, int gpio) {
    (void)gpio;
    if (!level && !sensors->usrfn->eisr && !sensors->usrfn->risr) {
        printf("Range Obj Detected\n");
    }
}

void *prgrm(t_sensors *sensors) {
#	ifdef DEBUG
        printf("Idle: %d Seq: %d Tick: %4.4f\n", isIdle(), sensors->usrfn->seqExecuting, (double)((gpioTick() - sensors->usrfn->startTick) / (double)1000000));
    #endif
    if (isIdle() || sensors->usrfn->eisr || sensors->usrfn->risr) {
        switch (sensors->usrfn->seqNext) {
            case 0: moveForward(sensors->mot, 10, 90);
                    sensors->usrfn->seqNext = 1;
                    sensors->usrfn->seqExecuting = 0;
                    break;

            case 1: moveBackwards(sensors->mot, 5, 50);
                    sensors->usrfn->seqNext = 2;
                    sensors->usrfn->seqExecuting = 1;
                    break;

            case 2:	moveForward(sensors->mot, 3, 10);
                    sensors->usrfn->seqNext = 0;
                    sensors->usrfn->seqExecuting = 2;
                    break;
            // ISR Edge Left
            case 10: moveBackwards(sensors->mot, 2, 95);
                    sensors->usrfn->seqNext = 11;
                    sensors->usrfn->seqExecuting = 10;
                    break;

            case 11: rotateRight(sensors->mot, 2, 95);
                    sensors->usrfn->seqExecuting = 11;
                    sensors->usrfn->seqNext = sensors->usrfn->seqPrev;
                    sensors->usrfn->eisr = 0;
                    break;
            // ISR Edge Right
            case 12: moveBackwards(sensors->mot, 2, 95);
                    sensors->usrfn->seqNext = 13;
                    sensors->usrfn->seqExecuting = 12;
                    break;

            case 13: rotateLeft(sensors->mot, 2, 95);
                    sensors->usrfn->seqExecuting = 13;
                    sensors->usrfn->seqNext = sensors->usrfn->seqPrev;
                    sensors->usrfn->eisr = 0;
                    break;
            // ISR Edge Back
            case 14: moveForward(sensors->mot, 2, 95);
                    sensors->usrfn->seqExecuting = 14;
                    sensors->usrfn->seqNext = sensors->usrfn->seqPrev;
                    sensors->usrfn->eisr = 0;
                    break;
            // Stop Sequence
            case 99: sensors->usrfn->seqExecuting = 99;
                    sensors->usrfn->seqRun = 0;
        }
    }

    #ifdef DISPLAY
        if (((gpioTick() - sensors->usrfn->startTick) / 1000000) % 2) {
            midStringDisplay("RUN", 6, GREEN, BLACK);
        }
        else {
            midStringDisplay("RUN", 6, DARK_GREEN, WHITE);
        }
    #endif

    return NULL;
}

int main(void) {
    t_sensors sensors;
    t_motor *motor = NULL;
	t_bno055 bno055;

	(void)bno055;
 	// if (gpioInitialise() < 0) {
    //     perror("pigpio failed initialisation!\n");
    //     syslog(LOG_DEBUG, "ERROR: gpioPWMInitialise (%s,%d)", __FILE__, __LINE__);
    //     return EXIT_FAILURE;
    // }

	// initGyro(&bno055);

	// while (1) {
	// 	getEul(&bno055);
	// 	gpioDelay(10);
	// }

    #ifdef WDT
        initWDT();
    #endif

    motor = (t_motor *)malloc(MOT * sizeof(t_motor));
    if (!motor) {
        perror ("Malloc for motor failed\n");
        syslog(LOG_DEBUG, "ERROR: gpioMotorMallocFailure (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    #ifdef DISPLAY
        if (initDisplay()) {
            return EXIT_FAILURE;
        }

        midStringDisplay("INIT", 4, BLUE, WHITE);
        sleep(2);
    #endif

    if (initSumo(&sensors, motor)) {
        return EXIT_FAILURE;
    }

    if (startSensors(&sensors)) {
        freeSumo(&sensors, motor);
        return EXIT_FAILURE;
    }

    #ifdef DISPLAY
        midStringDisplay("ARM", 4, YELLOW, BLACK);
        sleep(2);
    #endif

    if (initNavigation()) {
        freeSumo(&sensors, motor);
        return EXIT_FAILURE;
    }

    if (initSequence(edge, range, prgrm, sigPgrm, &sensors)) {
        freeSumo(&sensors, motor);
        return EXIT_FAILURE;
    }

    runSequence(1.0);

    freeNavigation();
    stopSensors(&sensors);
    freeSumo(&sensors, motor);
    freeDisplay();
    #ifdef WDT
        stopWDT();
    #endif

    return EXIT_SUCCESS;
}
