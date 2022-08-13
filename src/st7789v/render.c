// Copyright (c) 2021 Heewon Cho

#include "render.h"
#include <string.h>

void Render_Fill(uint8_t buff[], uint16_t color) {
    uint8_t pixel[] = { color >> 8, color & 0xFF };
    for (int cnt = 0 ; cnt < WIDTH * HEIGHT ; cnt++) {
        buff[cnt * 2] = pixel[0];
        buff[cnt * 2 + 1] = pixel[1];
    }
}

void Render_Pixel(uint8_t buff[], int x, int y, uint16_t color) {
    uint8_t pixel[] = { color >> 8, color & 0xFF };
    if (x >= WIDTH || y >= HEIGHT) {
        return;
    }
    buff[(y * WIDTH + x) * 2] = pixel[0];
    buff[(y * WIDTH + x) * 2 + 1] = pixel[1];
}

void Render_Char(uint8_t buff[], char text, int x, int y, int rot, int size, uint16_t color, const unsigned char font[], int fontsize) {
    switch (rot) {
        default:
        case 0:
            for (int ly = 0 ; ly < size * fontsize ; ly++) {
                for (int lx = 0 ; lx < size * fontsize ; lx++) {
                    if (font[text * fontsize + ly / size] & (0x80 >> lx / size)) {
                        Render_Pixel(buff, x + lx, y + ly, color);
                    }
                }
            }
            break;
        case 90:
            for (int ly = 0 ; ly < size * fontsize ; ly++) {
                for (int lx = 0 ; lx < size * fontsize ; lx++) {
                    if (font[text * fontsize + (fontsize  * size - ly) / size - 1] & (0x80 >> lx / size)) {
                        Render_Pixel(buff, x + ly, y + lx, color);
                    }
                }
            }
            break;
        case 180:
            for (int ly = 0 ; ly < size * fontsize ; ly++) {
                for (int lx = 0 ; lx < size * fontsize ; lx++) {
                    if (font[text * fontsize + (fontsize * size - ly) / size - 1] & (0x80 >> lx / size)) {
                        Render_Pixel(buff, x + lx, y + ly, color);
                    }
                }
            }
            break;
        case 270:
            for (int ly = 0 ; ly < size * fontsize ; ly++) {
                for (int lx = 0 ; lx < size * fontsize ; lx++) {
                    if (font[text * fontsize + ly / size] & (0x80 >> lx / size)) {
                        Render_Pixel(buff, x + ly, y + lx, color);
                    }
                }
            }
            break;
    }
}

void Render_String(uint8_t buff[], char *text, int x, int y, int rot, int size, uint16_t color, const unsigned char font[], int fontsize) {
    int cnt = 0;
    while (text[cnt] != '\0') {
        if (rot == 90)
            Render_Char(buff, text[cnt], (WIDTH - (fontsize * size)) - x, y + (cnt * fontsize * size), rot, size, color, font, fontsize);
        else
            Render_Char(buff, text[cnt], x + (cnt * fontsize * size), y + (fontsize * size), rot, size, color, font, fontsize);
        cnt++;
    }
}

void Render_String_Mid(uint8_t buff[], char *text, int rot, int size, uint16_t color, const unsigned char font[], int fontsize) {
    int cnt = 0;

    // (Width / 2) - ((fontsize * size) / 2.0) = X, (HEIGHT / 2) - ((size / 2) * (fontsize * size)) = Y
    int x = ((WIDTH - 1) / 2.0) - ((fontsize * size) / 2.0);
    int y = ((HEIGHT - 1) / 2.0) - ((size * fontsize / 2.0) * strlen(text));

    while (text[cnt] != '\0') {
        if (rot == 0)
            Render_Char(buff, text[cnt], x + (cnt * fontsize * size), y + (fontsize * size), rot, size, color, font, fontsize);
        else
            Render_Char(buff, text[cnt], (WIDTH - (fontsize * size)) - x, y + (cnt * fontsize * size), rot, size, color, font, fontsize);
        cnt++;
    }
}
