#ifndef WATCHDOG_H
#define WATCHDOG_H

#define WDT

#include "sumo.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

int initWDT(void);
void stopWDT(void);

#endif
