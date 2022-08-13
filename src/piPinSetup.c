#include "./includes/sumo.h"

/*
** RESERVED FOR DISPLAY
** GPIO 2,3,22,23,24,10,9,11,8,25
** 2  = I2C1 SDA
** 3  = I2C1 SCL
** 8  = SPI0 CE0
** 9  = SPI0 MISO
** 10 = SPI0 MOSI
** 11 = SPI0 SCLK
** 22 = Display Backlight
** 23 = Button 1
** 24 = Button 2
** 25 = Display IO
*/
void piPinSetup(t_sensors *sensors, t_motor *motors) {
    sensors->p_edge[LEFT] = 0; 		// Left Edge Sensor Pin 27
    sensors->p_edge[RIGHT] = 5; 	// Right Edge Sensor Pin 29
    sensors->p_edge[BACK] = 1; 		// Back Edge Sensor Pin 28
    sensors->p_range[LEFT] = 4;		// Left Range Sensor Pin 7
    sensors->p_range[RIGHT] = 14; 	// Right Range Sensor Pin 8
    sensors->p_range[BACK] = 15; 	// Back Range Sensor Pin 10
    sensors->p_range[FRONT] = 18; 	// Front Range Sensor Pin 12
    sensors->mot = motors;

    motors[LEFT].p_mtr[PWM] = 12;	// PWM Pin 32
    motors[LEFT].p_mtr[IN1] = 16;	// IN1 Pin 36
    motors[LEFT].p_mtr[IN2] = 20;	// IN2 Pin 38
    motors[LEFT].p_mtr[STBY] = 21;	// STBY Pin 40
    motors[RIGHT].p_mtr[PWM] = 13;	// PWM Pin 33
    motors[RIGHT].p_mtr[IN1] = 19;	// IN1 Pin 35
    motors[RIGHT].p_mtr[IN2] = 26;	// IN2 Pin 37
    motors[RIGHT].p_mtr[STBY] = 6;	// STBY Pin 31
    motors[LEFT].motor = LEFT;
    motors[RIGHT].motor = RIGHT;

    #ifdef DEBUG
        printf("Sensor Edge L: %d R: %d B: %d\n", sensors->p_edge[LEFT], sensors->p_edge[RIGHT], sensors->p_edge[BACK]);
        printf("Sensor Range: L: %d R: %d B: %d F: %d\n", sensors->p_range[LEFT], sensors->p_range[RIGHT], sensors->p_range[BACK], sensors->p_range[FRONT]);
        printf("Motor: %d IN1: %d IN2: %d PWM: %d STBY: %d\n", motors[LEFT].motor, motors[LEFT].p_mtr[IN1], motors[LEFT].p_mtr[IN2], motors[LEFT].p_mtr[PWM], motors[LEFT].p_mtr[STBY]);
        printf("Motor: %d IN1: %d IN2: %d PWM: %d STBY: %d\n", motors[RIGHT].motor, motors[RIGHT].p_mtr[IN1], motors[RIGHT].p_mtr[IN2], motors[RIGHT].p_mtr[PWM], motors[RIGHT].p_mtr[STBY]);
        fflush(0);
    #endif
}
