#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY

#include "sumo.h"
#include <st7789vw.h>
#include <render.h>
#include <syslog.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <bcm2835.h>

#define BLACK 0x0000
#define WHITE 0xffff
#define BLUE 0x001f
#define GREEN 0x07e0
#define DARK_GREEN 0x0300
#define YELLOW 0xffe0
#define RED 0xf800

int initDisplay(void);
int freeDisplay(void);
void clearDisplay(void);
void backLight(unsigned enable);
void midStringDisplay(char *str, int fontsize, uint16_t backgroundcolor, uint16_t fontcolor);

#endif
