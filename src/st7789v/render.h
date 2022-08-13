// Copyright (c) 2021 Heewon Cho

#ifndef RENDER_H
#define RENDER_H

#define WIDTH 135
#define HEIGHT 240

#define RGB565(r, g, b) ((r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3)

#include <stdint.h>

void Render_Fill(uint8_t buff[], uint16_t color);

void Render_Pixel(uint8_t buff[], int x, int y, uint16_t color);

void Render_Char(uint8_t buff[], char text, int x, int y, int rot, int size, uint16_t color, const unsigned char font[], int fontsize);
void Render_String(uint8_t buff[], char *text, int x, int y, int rot, int size, uint16_t color, const unsigned char font[], int fontsize);
void Render_String_Mid(uint8_t buff[], char *text, int rot, int size, uint16_t color, const unsigned char font[], int fontsize);

#endif
