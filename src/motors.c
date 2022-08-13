
#include "./includes/sumo.h"

/**
 * Motor Control Loop
 * 500 uS Loop
 * Low Level control of the Motor Controller Board
 **/
void *motorCtrl(void *mtr) {
    t_motor *motor;
    motor = (t_motor *)mtr;
    unsigned tdir = 2;
    unsigned tdut = 2;

    pthread_mutex_lock(&motor->m_mtr);
    while (motor->run) {
        if (tdir != motor->dir) {
            tdir = motor->dir;
            switch (motor->dir) {
                case 0:	gpioWrite(motor->p_mtr[IN1], PI_LOW);
                        gpioWrite(motor->p_mtr[IN2], PI_LOW);
                        break;

                case 1:	gpioWrite(motor->p_mtr[IN1], PI_HIGH);
                        gpioWrite(motor->p_mtr[IN2], PI_LOW);
                        break;

                case 2: gpioWrite(motor->p_mtr[IN1], PI_LOW);
                        gpioWrite(motor->p_mtr[IN2], PI_HIGH);
                        break;

                case 3: gpioWrite(motor->p_mtr[IN1], PI_HIGH);
                        gpioWrite(motor->p_mtr[IN2], PI_HIGH);
                        break;
            }
        }

        if (tdut != motor->dut && motor->softStart == 0) {
            tdut = motor->dut;
            if (motor->dut <= 100) {
                gpioPWM(motor->p_mtr[PWM], motor->dut);
            }
        }
        else {
            if (motor->dut != motor->softStart) {
                if (motor->dut <= 100) {
                    if (((gpioTick() - motor->startTick) / 1000000) % 2)  {
                        gpioPWM(motor->p_mtr[PWM], motor->softStart++);
                    }
                }
            }
            else {
                motor->softStart = 0;
            }
        }


        pthread_mutex_unlock(&motor->m_mtr);
        gpioDelay(500);
        pthread_mutex_lock(&motor->m_mtr);
    }
    pthread_mutex_unlock(&motor->m_mtr);
    #ifdef DEBUG
        printf("Motor %d Control Thread Exiting\n", motor->motor);
    #endif
    return NULL;
}

/**
 * Setup the motor control pins
 * Setup the PWM frequency and duty cycle
 * Start the Motor Control Loop
 **/
static int setupMotorPins(t_motor *motor) {
    int i;

    for (i = 0;i < 4; i++) {
        if (gpioSetMode(motor->p_mtr[i], PI_OUTPUT)) {
            perror("Motor pin setmode failed\n");
            syslog(LOG_DEBUG, "ERROR: gpioMotorSetPinModeFailure (%s,%d)", __FILE__, __LINE__);
            return EXIT_FAILURE;
        }
        gpioWrite(motor->p_mtr[i], PI_LOW);
    }

    if (gpioSetMode(motor->p_mtr[PWM], PI_ALT0)) {
        perror("Motor pin PWM setmode failed\n");
        syslog(LOG_DEBUG, "ERROR: gpioMotorSetPWMModeFailure (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }
    gpioSetPWMfrequency(motor->p_mtr[PWM], PWMFRQ);
    #ifdef DEBUG
        printf("Motor: %d PWM: %d Hz\n",motor->motor, gpioGetPWMfrequency(motor->p_mtr[PWM]));
    #endif
    gpioSetPWMrange(motor->p_mtr[PWM], 100); // set range of PWM to 0 to 100%
    gpioPWM(motor->p_mtr[PWM], 0);
    motor->run = 1; // set motor control loop to run
    return EXIT_SUCCESS;
}

/**
 * Initialize each motor
 * Setup mutex, start motor contorl thread
 **/
int initMotor(t_motor *motor) {
    if (pthread_mutex_init(&motor->m_mtr, NULL)) {
        perror("Motor Mutex Initialization Failed\n");
        syslog(LOG_DEBUG, "ERROR: gpioMotorMutexFailure (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }
    pthread_mutex_lock(&motor->m_mtr);
    if (setupMotorPins(motor)) {
        return EXIT_FAILURE;
    }
    motor->startTick = gpioTick();
    motor->pth_mtr = gpioStartThread(motorCtrl, motor);
    pthread_mutex_unlock(&motor->m_mtr);
    stopAllMotors(motor);
    return EXIT_SUCCESS;
}

/**
 * Free up motor resources
 * Stop the motor control thread
 * destroy mutexes
 **/
void freeMotor(t_motor *motor) {
    #ifdef DEBUG
        printf("Shutdown %d Motor\n", motor->motor);
    #endif
    pthread_mutex_lock(&motor->m_mtr);
    motor->run = 0;
    pthread_mutex_unlock(&motor->m_mtr);
    gpioStopThread(motor->pth_mtr);
    gpioDelay(100);
    pthread_mutex_destroy(&motor->m_mtr);
}

/**
 * Arm Motor
 */
void armMotor(t_motor *motor) {
    gpioWrite(motor->p_mtr[STBY], PI_HIGH);
}

/**
 * DisArm Motor
 **/
void disArmMotor(t_motor *motor) {
    gpioWrite(motor->p_mtr[STBY], PI_LOW);
}

/**
 * Is Motor Armed
 * Returns 1 if armed
 **/
int isArmed(t_motor *motor) {
    return gpioRead(motor->p_mtr[STBY]);
}

/**
 * Is Motor DisArmed
 * Returns 1 if disarmed
 **/
int isDisArmed(t_motor *motor) {
    return gpioRead(motor->p_mtr[STBY]);
}

void setSoftStart(t_motor *motor, int duty) {
    pthread_mutex_lock(&motor->m_mtr);
    motor->softStart = 1;
    motor->dut = duty;
    pthread_mutex_unlock(&motor->m_mtr);
}
/**
 * Common to Both Motors
 * The Below Function operatee on Both Motors
 * The Routines should not be called directly
 */

/*
** Do Not Call Directly
** Use Navigation instead
**/
void stopAllMotors(t_motor *motor) {
    pthread_mutex_lock(&motor->m_mtr);
    motor[LEFT].dir = 0;
    motor[RIGHT].dir = 0;
    motor[LEFT].dut = 0;
    motor[RIGHT].dut = 0;
    pthread_mutex_unlock(&motor->m_mtr);
}

/**
 * Do Not Call Directly
 * Use Naviation instead
 * Writes directly to the GPIO
 * Stops the Motor Control Loop
 **/
void killAllIO(t_motor *motor) {
    gpioWrite(motor[LEFT].p_mtr[STBY], PI_LOW);
    gpioWrite(motor[RIGHT].p_mtr[STBY], PI_LOW);

    gpioWrite(motor[LEFT].p_mtr[IN1], PI_LOW);
    gpioWrite(motor[LEFT].p_mtr[IN2], PI_LOW);
    gpioWrite(motor[RIGHT].p_mtr[IN1], PI_LOW);
    gpioWrite(motor[RIGHT].p_mtr[IN2], PI_LOW);
    gpioPWM(motor[LEFT].p_mtr[PWM], 0);
    gpioPWM(motor[RIGHT].p_mtr[PWM], 0);
}

/**
* Do Not Call Directly
* Use Navigation instead
* Stops the Motor Control Loop
**/
void estopAllMotors(t_motor *motor) {
    disArmMotor(&motor[LEFT]);
    disArmMotor(&motor[RIGHT]);
    pthread_mutex_lock(&motor->m_mtr);
    motor[LEFT].dir = 0;
    motor[RIGHT].dir = 0;
    motor[LEFT].run = 0;
    motor[RIGHT].run = 0;
    pthread_mutex_unlock(&motor->m_mtr);
}
