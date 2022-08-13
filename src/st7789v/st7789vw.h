// Copyright (c) 2021 Heewon Cho

#ifndef ST7789VW_H
#define ST7789VW_H

#define ST7789VW_NOP			0x00
#define ST7789VW_SWRESET		0x01
#define ST7789VW_RDDID	    	0x04
#define ST7789VW_RDDST	    	0x09
#define ST7789VW_RDDPM	    	0x0A
#define ST7789VW_RDDMADCTL  	0x0B
#define ST7789VW_RDDCOLMOD  	0x0C
#define ST7789VW_RDDIM	    	0x0D
#define ST7789VW_RDDSM	    	0x0E
#define ST7789VW_RDDSDR	    	0x0F
#define ST7789VW_SLPIN	    	0x10
#define ST7789VW_SLPOUT	    	0x11
#define ST7789VW_PTLON	    	0x12
#define ST7789VW_NORON	    	0x13
#define ST7789VW_INVOFF	    	0x20
#define ST7789VW_INVON	    	0x21
#define ST7789VW_GAMSET	    	0x26
#define ST7789VW_DISPOFF		0x28
#define ST7789VW_DISPON	    	0x29
#define ST7789VW_CASET	    	0x2A
#define ST7789VW_RASET	    	0x2B
#define ST7789VW_RAMWR	    	0x2C
#define ST7789VW_RAMRD	    	0x2E
#define ST7789VW_PTLAR	    	0x30
#define ST7789VW_VSCRDEF		0x33
#define ST7789VW_TEOFF	    	0x34
#define ST7789VW_TEON			0x35
#define ST7789VW_MADCTL	    	0x36
#define ST7789VW_VSCSAD	    	0x37
#define ST7789VW_IDMOFF	    	0x38
#define ST7789VW_IDMON	    	0x39
#define ST7789VW_COLMOD	    	0x3A
#define ST7789VW_WRMEMC	    	0x3C
#define ST7789VW_RDMEMC	    	0x3E
#define ST7789VW_STE			0x44
#define ST7789VW_GSCAN  		0x45
#define ST7789VW_WRDISBV		0x51
#define ST7789VW_RDDISBV		0x52
#define ST7789VW_WRCTRLD		0x53
#define ST7789VW_RDCTRLD   		0x54
#define ST7789VW_WRCACE	    	0x55
#define ST7789VW_RDCABC	    	0x56
#define ST7789VW_WRCABCMB		0x5E
#define ST7789VW_RDCABCMB		0x5F
#define ST7789VW_RDABCSDR		0x68
#define ST7789VW_RDID1	    	0xDA
#define ST7789VW_RDID2	    	0xDB
#define ST7789VW_RDID3	    	0xDC
#define ST7789VW_RAMCTRL		0xB0
#define ST7789VW_RGBCTRL		0xB1
#define ST7789VW_PORCTRL		0xB2
#define ST7789VW_FRCTRL1		0xB3
#define ST7789VW_PARCTRL		0xB5
#define ST7789VW_GCTRL	    	0xB7
#define ST7789VW_GTADJ	    	0xB8
#define ST7789VW_DGMEN	    	0xBA
#define ST7789VW_VCOMS	    	0xBB
#define ST7789VW_POWSAVE		0xBC
#define ST7789VW_DLPOFFSAVE 	0xBD
#define ST7789VW_LCMCTRL		0xC0
#define ST7789VW_IDSET	    	0xC1
#define ST7789VW_VDVVRHEN		0xC2
#define ST7789VW_VRHS			0xC3
#define ST7789VW_VDVS			0xC4
#define ST7789VW_VCMOFSET		0xC5
#define ST7789VW_FRCTRL2		0xC6
#define ST7789VW_CABCCTRL		0xC7
#define ST7789VW_REGSEL1		0xC8
#define ST7789VW_REGSEL2		0xCA
#define ST7789VW_PWMFRSEL		0xCC
#define ST7789VW_PWCTRL1		0xD0
#define ST7789VW_VAPVANEN		0xD2
#define ST7789VW_CMD2EN	    	0xDF
#define ST7789VW_PVGAMCTRL  	0xE0
#define ST7789VW_NVGAMCTRL  	0xE1
#define ST7789VW_DGMLUTR		0xE2
#define ST7789VW_DGMLUTB		0xE3
#define ST7789VW_GATECTRL		0xE4
#define ST7789VW_SPI2EN	    	0xE7
#define ST7789VW_PWCTRL2		0xE8
#define ST7789VW_EQCTRL	    	0xE9
#define ST7789VW_PROMCTRL		0xEC
#define ST7789VW_PROMEN	    	0xFA
#define ST7789VW_NVMSET	    	0xFC
#define ST7789VW_PROMACT		0xFE

#define ST7789VW_COLOR_MODE_16BIT 0x55
#define ST7789VW_COLOR_MODE_18BIT 0x66

#define ST7789VW_CS_PIN RPI_BPLUS_GPIO_J8_24
#define ST7789VW_DC_PIN RPI_BPLUS_GPIO_J8_22
#define ST7789VW_BACKLIGHT_PIN RPI_BPLUS_GPIO_J8_15

#define ST7789VW_FR_60HZ 0x0F

#define ST7789VW_WIDTH 135
#define ST7789VW_HEIGHT 240

#define X_SHIFT 52
#define Y_SHIFT 40

#define ROTATION 90

#include <bcm2835.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

void ST7789VW_WriteCmd(uint8_t cmd);
void ST7789VW_WriteByte(uint8_t data);
void ST7789VW_WriteData(uint8_t *buff, size_t size);
void ST7789VW_EnableBackLight(void);
void ST7789VW_DisableBackLight(void);

int ST7789VW_Init();
void ST7789VW_Stop();

void ST7789VW_SetAddress(int xs, int ys, int xe, int ye);

void ST7789VW_Draw(uint8_t buff[]);

#endif
