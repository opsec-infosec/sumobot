#include "./includes/display.h"
#include "./includes/VGA8x8.h"

uint8_t *buffer;
pthread_t render;
pthread_mutex_t m_render;

/**
 * Dipslay Render
 * Thread to redraw the display every 250 uSeconds
 **/
static void *gfxRender() {
	while (1) {
		pthread_mutex_lock(&m_render);
		ST7789VW_Draw(buffer);
		pthread_mutex_unlock(&m_render);
		bcm2835_delay(250);
	}
	return NULL;
}

/**
 * Initialize Display
 * Setup SPI Bus, allocate display buffer, and setup display mutex
 **/
int initDisplay(void) {
    if (ST7789VW_Init()) {
		perror("Display ST877899VW Faile to Initalize\n");
		syslog(LOG_DEBUG, "ERROR: ST87789vwInitFailure (%s,%d)", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

	if (pthread_mutex_init(&m_render, NULL)) {
		perror("Failed to init Mutex Display Render\n");
		syslog(LOG_DEBUG, "ERROR: displayRenderMutexInitFailed (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	buffer = (uint8_t *)malloc(ST7789VW_WIDTH * ST7789VW_HEIGHT * 2);
	if (!buffer) {
		perror("Failed to Malloc Display Buffer\n");
		syslog(LOG_DEBUG, "ERROR: ST87789vwBufferMallocFailure (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	for (int i = 0; i <= (ST7789VW_WIDTH * ST7789VW_HEIGHT * 2); i++) {
		buffer[i] = 0x00;
	}

	if (pthread_create(&render, NULL, &gfxRender, NULL)) {
		perror("Failed to create Display Render Thread\n");
		syslog(LOG_DEBUG, "ERROR: displayRenderThreadFailed (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	ST7789VW_EnableBackLight();
	return EXIT_SUCCESS;
}

/**
 * Free Display Buffer and mutex
 * Clears display, turns off the backlight, stops the render thread
 * Frees the buffer memeory allocation and stops the SPI Bus
 **/
int freeDisplay(void) {
	clearDisplay();
	ST7789VW_DisableBackLight();
	pthread_cancel(render);
	if (buffer) {
		free(buffer);
		buffer = NULL;
	}

	pthread_mutex_destroy(&m_render);
	ST7789VW_Stop();
	#ifdef DEBUG
		printf("Display Free Finished\n");
	#endif
	return EXIT_SUCCESS;
}

/**
 * Display String centered within the dipslay
 **/
void midStringDisplay(char *str, int fontsize, uint16_t backgroundcolor, uint16_t fontcolor) {
	pthread_mutex_lock(&m_render);
	Render_Fill(buffer, backgroundcolor);
	Render_String_Mid(buffer, str, 90, fontsize, fontcolor, VGA8x8, 8);
	pthread_mutex_unlock(&m_render);
}

/**
 * Clear the Display
 **/
void clearDisplay(void) {
	pthread_mutex_lock(&m_render);
	Render_Fill(buffer, BLACK);
	pthread_mutex_unlock(&m_render);
}

/**
 * Turn on and off the Display Backlight
 * setting enable to 1 turns on the backlight
 **/
void backLight(unsigned enable) {
	if (enable)
		ST7789VW_EnableBackLight();
	else
		ST7789VW_DisableBackLight();
}
