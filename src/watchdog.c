#include "./includes/watchdog.h"

int fd;
pthread_t th_wdt;

/**
 * Watchdog Timer Thread
 * This resets the timer every 30 seconds
 * to avoid timour of the timer and reboot
 **/
static void *petWDT() {
    while (1) {
        ioctl(fd, WDIOC_KEEPALIVE, NULL);
        sleep(30);
    }
    return NULL;
}

/**
 * WatchDog Timer - Disable during debugging!
 * Creates a thread that "pats" the dog
 * every 30 seconds.  If this thread
 * dies, the controller will rebooot
 **/
int initWDT(void) {
    #ifdef DEBUG
        printf("Starting WDT\n");
    #endif

    if (pthread_create(&th_wdt, NULL, &petWDT, NULL)) {
        perror("Failed to create WDTr Thread\n");
        syslog(LOG_DEBUG, "ERROR: WDTThreadFailed (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    fd = open("/dev/watchdog", O_RDWR);
    if (fd == -1) {
        perror("Error Opening WDT\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * Watchdog Timer Stop
 **/
void stopWDT(void) {
    write(fd, "V", 1);
    close(fd);
    #ifdef DEBUG
        printf("Stopping WDT\n");
    #endif
}
