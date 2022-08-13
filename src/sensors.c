
#include "./includes/sumo.h"
#include "./includes/sequence.h"

/**
 * Edge Sensor Interrupt Service Routine
 **/
void edgeISR(int gpio, int level, unsigned tick, void *data) {
    t_sensors *sensors;
    sensors = (t_sensors *)data;
    int i = 0;
    (void)tick;

    for (i = 0; i < EDGE; i++) {
        if (gpio == sensors->p_edge[i]) {
            pthread_mutex_lock(&sensors->m_edge[i]);
            sensors->edge[i] = level;
            pthread_mutex_unlock(&sensors->m_edge[i]);
            #ifdef DEBUG
                printf("PIN: %d LEVEL: %d TICK: %4.4f\n", sensors->p_edge[i], level, (double)((tick - sensors->usrfn->startTick) / (double)1000000));
            #endif
            #ifdef SEQUENCE_H
                if (sensors->usrfn->fEdge)
                    (*(sensors->usrfn->fEdge))(sensors, level, gpio);
            #endif
            break;
        }
    }
}

/**
 * Range Sensor Interrupt Service Routine
 **/
void rangeISR(int gpio, int level, unsigned tick, void *data) {
    t_sensors *sensors;
    sensors = (t_sensors *)data;
    int i = 0;
    (void)tick;

    for (i = 0; i < RANGE; i++) {
        if (gpio == sensors->p_range[i]) {
            pthread_mutex_lock(&sensors->m_range[i]);
            sensors->range[i] = level;
            pthread_mutex_unlock(&sensors->m_range[i]);
            #ifdef DEBUG
                printf("PIN: %d LEVEL: %d TICK: %4.4f\n", sensors->p_range[i], level, (double)((tick - sensors->usrfn->startTick) / (double)1000000));
            #endif
            #ifdef SEQUENCE_H
                if (sensors->usrfn->fRange) {
                    (*(sensors->usrfn->fRange))(sensors, level, gpio);
                }
            #endif
            break;
        }
    }
}

/**
 * Free up Sensor Resources
 **/
static void freeSensors(t_sensors *sensors) {
    int i;

    for (i = 0; i < EDGE; i++)
        pthread_mutex_destroy(&sensors->m_edge[i]);

    for (i = 0; i < RANGE; i++)
        pthread_mutex_destroy(&sensors->m_range[i]);

    gpioDelay(100);
    free(sensors->range);
    free(sensors->m_range);
    free(sensors->edge);
    free(sensors->m_edge);
    sensors->range = NULL;
    sensors->m_range = NULL;
    sensors->edge = NULL;
    sensors->m_edge = NULL;
}

/**
 * Allocate and initialize Sensors
 * Allocate memory for data buffers and mutexes
 **/
static int allocSensors(t_sensors *sensors) {
    sensors->m_edge = (pthread_mutex_t *)malloc(EDGE * sizeof(pthread_mutex_t));
    if (!sensors->m_edge) {
        perror("Failed to Malloc Edge Mutex\n");
        syslog(LOG_DEBUG, "ERROR: gpioEdgeMallocMutexFailed (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
        return EXIT_FAILURE;
    }
    sensors->edge = (unsigned *)malloc(EDGE * sizeof(unsigned));
    if (!sensors->edge) {
        perror("Failed to Malloc Edge Sensor Array\n");
        syslog(LOG_DEBUG, "ERROR: gpioEdgeMallocSensorFailed (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
        return EXIT_FAILURE;
    }
    sensors->m_range = (pthread_mutex_t *)malloc(RANGE * sizeof(pthread_mutex_t));
    if (!sensors->m_range) {
        perror("Failed to Malloc Range  Mutex\n");
        syslog(LOG_DEBUG, "ERROR: gpioRangeMallocMutexFailed (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
        return EXIT_FAILURE;
    }
    sensors->range = (unsigned *)malloc(RANGE * sizeof(unsigned));
    if (!sensors->range) {
        perror("Failed to Malloc Range Sensor Array\n");
        syslog(LOG_DEBUG, "ERROR: gpioRangeMallocSensorFailed (%s,%d)", __FILE__, __LINE__);
        gpioTerminate();
    }
    return EXIT_SUCCESS;
}

/**
 * Start the Edge Sensor
 **/
static void startEdgeSensor(t_sensors *sensors) {
    int i;

    for (i = 0; i < EDGE; i++) {
        if (pthread_mutex_init(&sensors->m_edge[i], NULL)) {
            perror("Failed to init Mutex edge\n");
            syslog(LOG_DEBUG, "ERROR: gpioEdgeMutexInitFailed (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        pthread_mutex_lock(&sensors->m_edge[i]);
        if (gpioSetMode(sensors->p_edge[i], PI_INPUT)) {
            perror("Failed to set pin mode edge\n");
            syslog(LOG_DEBUG, "ERROR: gpioEdgePinModeFailed (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        if (gpioSetPullUpDown(sensors->p_edge[i], PI_PUD_OFF)) {
            perror("Failed to set pull down mode edge\n");
            syslog(LOG_DEBUG, "ERROR: gpioEdgePullResistorMode (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        if (gpioSetISRFuncEx(sensors->p_edge[i], EITHER_EDGE, 0, edgeISR, sensors)) {
            perror("Failed to set ISR edge\n");
            syslog(LOG_DEBUG, "ERROR: gpioEdgeFailedISR (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        sensors->edge[i] = 0;
    }
    for (i = 0; i < EDGE; i++)
        pthread_mutex_unlock(&sensors->m_edge[i]);
}

/**
 * Start the Range Sensor
 **/
static void startRangeSensors(t_sensors *sensors) {
    int i;

    for (i = 0; i < RANGE; i++) {
        if (pthread_mutex_init(&sensors->m_range[i], NULL)) {
            perror("Failed to init Mutex range\n");
            syslog(LOG_DEBUG, "ERROR: gpioRangeMutexInitFailed (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        pthread_mutex_lock(&sensors->m_range[i]);
        if (gpioSetMode(sensors->p_range[i], PI_INPUT)) {
            perror("Failed to set pin mode range");
            syslog(LOG_DEBUG, "ERROR: gpioRangePinModeFailed (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        if (gpioSetPullUpDown(sensors->p_range[i], PI_PUD_OFF)) {
            perror("Failed to set pull down mode range\n");
            syslog(LOG_DEBUG, "ERROR: gpioRangePullResistorMode (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        if (gpioSetISRFuncEx(sensors->p_range[i], EITHER_EDGE, 0, rangeISR, sensors)) {
            perror("Failed to set ISR range\n");
            syslog(LOG_DEBUG, "ERROR: gpioRangeFailedISR (%s,%d)", __FILE__, __LINE__);
            gpioTerminate();
            return EXIT_FAILURE;
        }
        sensors->range[i] = 0;
    }
    for (i = 0; i < RANGE; i++)
        pthread_mutex_unlock(&sensors->m_range[i]);
}

/**
 * Start up all Sensors
 **/
int startSensors(t_sensors *sensors) {
    int i = 0;

    if (allocSensors(sensors)) {
        return EXIT_FAILURE;
    }

    startEdgeSensor(sensors);
    startRangeSensors(sensors);

    #ifdef DEBUG
        printf("Start Sensors Finished\n");
    #endif
    return EXIT_SUCCESS;
}

/**
 * Stop all sensors and free resources
 **/
void stopSensors(t_sensors *sensors) {
    int i = 0;

    for (i = 0; i < EDGE; i++)
        gpioSetISRFuncEx(sensors->p_edge[i], EITHER_EDGE, 0, NULL, sensors);

    for (i = 0; i < RANGE; i++)
        gpioSetISRFuncEx(sensors->p_range[i], EITHER_EDGE, 0, NULL, sensors);

    gpioDelay(100);
    freeSensors(sensors);

    #ifdef DEBUG
        printf("Stop Sensors Finished\n");
    #endif
}
