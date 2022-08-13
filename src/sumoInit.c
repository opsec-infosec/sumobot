#include "./includes/sumo.h"
#include "./includes/display.h"

/**
 * Initialize all sensors and motors
 * for the Sumo Bot
 **/
int initSumo(t_sensors *sensors, t_motor *motor) {
    printf(VERSION);

    piPinSetup(sensors, motor);

    gpioCfgClock(4, 0, 0);
    if (gpioInitialise() < 0) {
        perror("pigpio failed initialisation!\n");
        syslog(LOG_DEBUG, "ERROR: gpioPWMInitialise (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }
    printf("pigpio initalized\n");

    if (initMotor(&motor[LEFT])) {
        return EXIT_FAILURE;
    }
    if (initMotor(&motor[RIGHT])) {
        return EXIT_FAILURE;
    }
    armMotor(&motor[LEFT]);
    armMotor(&motor[RIGHT]);
    #ifdef DEBUG
        printf("Sumo Init Finished\n");
    #endif
    return EXIT_SUCCESS;
}

/**
 * Stop all sensors and motors and
 * free up resources for the Sumo Bot
 **/
int freeSumo(t_sensors *sensors, t_motor *motor) {
    motor[LEFT].run = 0;
    motor[RIGHT].run = 0;
    killAllIO(motor);
    if (sensors->mot != NULL) {
        freeMotor(&sensors->mot[LEFT]);
        freeMotor(&sensors->mot[RIGHT]);
        sensors->mot = NULL;
    }
    if (motor != NULL) {
        free(motor);
        motor = NULL;
    }
    time_sleep(1.0);
    gpioTerminate();

    #ifdef DEBUG
        printf("Sumo Free Finished\n");
    #endif
    return EXIT_SUCCESS;
}
