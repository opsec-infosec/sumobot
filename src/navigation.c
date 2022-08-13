#include "./includes/sumo.h"

/*
** motors = motor cntrl structure
** control =
**	0 = idle
** 	1 = Forward
**	2 = Backwards
**	3 = Turn Left
**	4 = Turn Right
**	5 = Rotate Left
**	6 = Rotate Right
**	7 = Stop
**	8 = Emergency Stop
*/
typedef struct s_navi {
    t_motor *motors;
    unsigned control;
    double seconds;
    unsigned angle;
    unsigned speed;
    unsigned idle;
}t_navi;

static	pthread_t *th_worker;
static	pthread_mutex_t	m_worker;
static	t_navi navi;

static void moveFinish(t_navi *navigation) {
    time_sleep(navigation->seconds);
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dir = STOP;
    navigation->motors[RIGHT].dir = STOP;
    navigation->motors[LEFT].dut = 0;
    navigation->motors[RIGHT].dut = 0;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    navigation->idle = 1;
}

static void fwd(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = navigation->speed;
    navigation->motors[RIGHT].dut = navigation->speed;
    navigation->motors[LEFT].dir = CW;
    navigation->motors[RIGHT].dir = CW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);

}

static void bkw(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = navigation->speed;
    navigation->motors[RIGHT].dut = navigation->speed;
    navigation->motors[LEFT].dir = CCW;
    navigation->motors[RIGHT].dir = CCW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);
}

static void tr(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = navigation->speed;
    navigation->motors[RIGHT].dut = (navigation->speed - 10) - navigation->angle;
    navigation->motors[LEFT].dir = CW;
    navigation->motors[RIGHT].dir = CW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);
}

static void tl(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = (navigation->speed - 10) - navigation->angle;
    navigation->motors[RIGHT].dut = navigation->speed;
    navigation->motors[LEFT].dir = CW;
    navigation->motors[RIGHT].dir = CW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);
}

static void rr(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = navigation->speed;
    navigation->motors[RIGHT].dut = navigation->speed;
    navigation->motors[LEFT].dir = CW;
    navigation->motors[RIGHT].dir = CCW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);
}

static void rl(t_navi *navigation) {
    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    navigation->motors[LEFT].dut = navigation->speed;
    navigation->motors[RIGHT].dut = navigation->speed;
    navigation->motors[LEFT].dir = CCW;
    navigation->motors[RIGHT].dir = CW;
    pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
    pthread_mutex_unlock(&m_worker);
    moveFinish(navigation);
}


void *worker(void *nav) {
    t_navi *navigation;
    navigation = (t_navi *)nav;

    pthread_mutex_lock(&m_worker);
    pthread_mutex_lock(&navigation->motors[LEFT].m_mtr);
    pthread_mutex_lock(&navigation->motors[RIGHT].m_mtr);
    if (navigation->motors[LEFT].run && navigation->motors[RIGHT].run) {
        pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
        pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
        pthread_mutex_unlock(&m_worker);

        switch (navigation->control) {
            case 1: fwd(navigation);
                    break;

            case 2: bkw(navigation);
                    break;

            case 3: tl(navigation);
                    break;

            case 4: tr(navigation);
                    break;

            case 5: rl(navigation);
                    break;

            case 6: rr(navigation);
                    break;

            default:perror("Unknown Navigation Command\n");
                    break;
        }
    }
    else {
        pthread_mutex_unlock(&navigation->motors[LEFT].m_mtr);
        pthread_mutex_unlock(&navigation->motors[RIGHT].m_mtr);
        pthread_mutex_unlock(&m_worker);
        perror("Motor Run Thread is Dead\n");
    }
    return NULL;
}

static void sSthread(unsigned start) {
    if (th_worker) {
        pthread_mutex_lock(&m_worker);
        gpioStopThread(th_worker);
        pthread_mutex_unlock(&m_worker);
        th_worker = NULL;
        if (start) {
            th_worker = gpioStartThread(worker, &navi);
        }
    }
    else {
        if (start) {
            th_worker = gpioStartThread(worker, &navi);
        }
    }
}

void stop(t_motor *motor) {
    sSthread(0);
    stopAllMotors(motor);
}

void emergencyStop(t_motor *motor) {
    sSthread(0);
    estopAllMotors(motor);
}

void moveForward(t_motor *motor, unsigned seconds, unsigned speed) {
    navi.control = 1;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.idle = 0;

    sSthread(1);
}

void moveBackwards(t_motor *motor, unsigned seconds, unsigned speed) {
    navi.control = 2;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.idle = 0;

    sSthread(1);
}


void turnLeft(t_motor *motor, unsigned seconds, unsigned angle, unsigned speed) {
    navi.control = 3;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.angle = angle;
    navi.idle = 0;

    sSthread(1);
}

void turnRight(t_motor *motor, unsigned seconds, unsigned angle, unsigned speed) {
    navi.control = 4;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.angle = angle;
    navi.idle = 0;

    sSthread(1);
}

void rotateLeft(t_motor *motor, unsigned seconds, unsigned speed) {
    navi.control = 5;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.idle = 0;

    sSthread(1);
}

void rotateRight(t_motor *motor, unsigned seconds, unsigned speed) {
    navi.control = 6;
    navi.seconds = seconds;
    navi.motors = motor;
    navi.speed = speed;
    navi.idle = 0;

    sSthread(1);
}

int initNavigation(void) {
    navi.idle = 1;
    if (pthread_mutex_init(&m_worker, NULL)) {
        perror("Error Initializing Navigation mutex\n");
        syslog(LOG_DEBUG, "ERROR: gpioNavigationMutexFailure (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
        return EXIT_FAILURE;
    }
    #ifdef DEBUG
        printf("Navigation Init Finished\n");
    #endif
    return EXIT_SUCCESS;
}

void freeNavigation(void) {
    if (th_worker) {
        gpioStopThread(th_worker);
        gpioDelay(50);
    }
    pthread_mutex_destroy(&m_worker);
    #ifdef DEBUG
        printf("Navigation Free Finished\n");
    #endif
}

int isIdle(void) {
    return navi.idle;
}
