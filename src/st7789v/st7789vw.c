// Copyright (c) 2021 Heewon Cho

#include "st7789vw.h"

void ST7789VW_WriteCmd(uint8_t cmd) {
    bcm2835_gpio_write(ST7789VW_CS_PIN, LOW);
    bcm2835_gpio_write(ST7789VW_DC_PIN, LOW);
    bcm2835_spi_transfer(cmd);
    bcm2835_gpio_write(ST7789VW_CS_PIN, HIGH);
}

void ST7789VW_WriteByte(uint8_t data) {
    bcm2835_gpio_write(ST7789VW_CS_PIN, LOW);
    bcm2835_gpio_write(ST7789VW_DC_PIN, HIGH);
    bcm2835_spi_transfer(data);
    bcm2835_gpio_write(ST7789VW_CS_PIN, HIGH);
}

void ST7789VW_WriteData(uint8_t *buff, size_t size) {
    bcm2835_gpio_write(ST7789VW_CS_PIN, LOW);
    bcm2835_gpio_write(ST7789VW_DC_PIN, HIGH);
    bcm2835_spi_writenb((char *)buff, size);
    bcm2835_gpio_write(ST7789VW_CS_PIN, HIGH);
}

int ST7789VW_Init() {

    if (!bcm2835_init()) {
        perror("BCM2835 not initialized\n");
        syslog(LOG_DEBUG, "ERROR: ST87789vwBCMInitFailed (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);

    bcm2835_gpio_fsel(ST7789VW_CS_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ST7789VW_DC_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ST7789VW_BACKLIGHT_PIN, BCM2835_GPIO_FSEL_OUTP);

    ST7789VW_WriteCmd(ST7789VW_SWRESET);
    bcm2835_delay(120);

    ST7789VW_WriteCmd(ST7789VW_FRCTRL2);
    ST7789VW_WriteByte(ST7789VW_FR_60HZ);

    ST7789VW_WriteCmd(ST7789VW_COLMOD);
    ST7789VW_WriteByte(ST7789VW_COLOR_MODE_16BIT);

    ST7789VW_WriteCmd(ST7789VW_INVON);

    ST7789VW_WriteCmd(ST7789VW_SLPOUT);
    bcm2835_delay(120);

    ST7789VW_WriteCmd(ST7789VW_DISPON);

    return EXIT_SUCCESS;
}

void ST7789VW_SetAddress(int xs, int ys, int xe, int ye) {

    xs += X_SHIFT;
    xe += X_SHIFT;
    ys += Y_SHIFT;
    ye += Y_SHIFT;

    ST7789VW_WriteCmd(ST7789VW_CASET);
    ST7789VW_WriteByte((xs >> 8) & 0xFF);
    ST7789VW_WriteByte((xs) & 0xFF);
    ST7789VW_WriteByte((xe >> 8) & 0xFF);
    ST7789VW_WriteByte((xe) & 0xFF);

    ST7789VW_WriteCmd(ST7789VW_RASET);
    ST7789VW_WriteByte((ys >> 8) & 0xFF);
    ST7789VW_WriteByte((ys) & 0xFF);
    ST7789VW_WriteByte((ye >> 8) & 0xFF);
    ST7789VW_WriteByte((ye) & 0xFF);

}

void ST7789VW_Stop() {
    bcm2835_spi_end();
}

void ST7789VW_Draw(uint8_t buff[]) {
    ST7789VW_SetAddress(0, 0, ST7789VW_WIDTH - 1, ST7789VW_HEIGHT - 1);
    ST7789VW_WriteCmd(ST7789VW_RAMWR);

    ST7789VW_WriteData(buff, ST7789VW_WIDTH * ST7789VW_HEIGHT * 2);
}

void ST7789VW_EnableBackLight(void) {
    bcm2835_gpio_write(ST7789VW_BACKLIGHT_PIN, HIGH);
}

void ST7789VW_DisableBackLight(void) {
    bcm2835_gpio_write(ST7789VW_BACKLIGHT_PIN, LOW);
}
