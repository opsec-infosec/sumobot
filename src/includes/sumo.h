
#ifndef SUMO_H
#define SUMO_H

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <pigpio.h>

#define VERSION "SUMO BOT v1.0a\n"

#define PWMFRQ 10000

#define EDGE 3
#define RANGE 4
#define MOT 2

//#define DEBUG

/*
** Sensor Enum
*/
enum pos {
    LEFT,
    RIGHT,
    BACK,
    FRONT
};


/*
** Motor Enum
*/
enum sig {
    IN1,
    IN2,
    PWM,
    STBY
};

enum dir {
    STOP,
    CW,
    CCW,
    BRK
};

/*
** Low Level Motor Driver Structure
** TB6612FNG Motor Driver
** --------------------------------
** p_mtr[4] = GPIO Pins for Motor Driver, 0 = In1, 1 = In2, 2 = PWM, 3 = STBY
** IN1	IN2	PWM		STBY	Mode
** -------------------------------------
** H 	H 	H/L 	H 		Short brake
** L 	H 	H 		H 		CCW
** L	H	L		H		Short brake
** H	L	H		H		CW
** H	L	L		H		Short brake
** L 	L 	H 		H 		Stop
** H/L 	H/L	H/L 	L 		Standby
**=======================================
** dir = Direction of Motor, 0 Stop 1 CW 0 2 CCW 3 Brake
** dut = PWM dury cycle, 0 to 100%
** softStart = 0 Disable, >0 = duty cycle to ramp to, this number will increas
**  until the dut is reached
** stby = standby motor driver, 0 standby, 1 active
** pth_mtr = pthread
** m_mtr = pthread mutex
** run = run motor thread 1 = run 0 = kill
** idle = motor is idels 1 = idle, 0 = not idle
*/
typedef struct s_motor {
    unsigned motor;
    unsigned p_mtr[4];
    unsigned dir;
    unsigned dut;
    unsigned softStart;
    unsigned run;
    uint32_t startTick;
    pthread_t *pth_mtr;
    pthread_mutex_t	m_mtr;

}t_motor;

/*
** Sensors Structure
** -----------------
** Range Sensors
** p_range[] = pins
** range[] = IO Values
** m_range = pthread mutex array
**	0 = Left
**	1 = Right
**	2 = Back
**	3 = Front
**
** Edge Sensors
** p_edge[] = pins
** edge[] = IO Values
** m_edge = pthread mutex array
** 	0 = Left
**	1 = Right
**	2 = Back
**
** mot pointer to motor structure 0 = Left, 1 = Right
*/
typedef struct s_sensors {
    int p_range[RANGE];
    int p_edge[EDGE];
    unsigned *range;
    unsigned *edge;
    pthread_mutex_t *m_range;
    pthread_mutex_t *m_edge;
    t_motor	*mot;
    struct s_usrfunc *usrfn;
}t_sensors;


int initSumo(t_sensors *sensors, t_motor *motor);
int freeSumo(t_sensors *sensors, t_motor *motor);
void piPinSetup(t_sensors *sensors, t_motor *motors);
int startSensors(t_sensors *sensors);
void stopSensors(t_sensors *sensors);
int initMotor(t_motor *motors);
void freeMotor(t_motor *motors);
void estopAllMotors(t_motor *motor);
void stopAllMotors(t_motor *motor);
void armMotor(t_motor *motor);
int  isArmed(t_motor *motor);
int  isDisArmed(t_motor *motor);
void disArmMotor(t_motor *mmotor);
void killAllIO(t_motor *motor);
void setSoftStart(t_motor *motor, int duty);

int initNavigation(void);
void freeNavigation(void);
int	isIdle(void);
void moveForward(t_motor *motor, unsigned seconds, unsigned speed);\
void moveBackwards(t_motor *motor, unsigned seconds, unsigned speed);
void stop(t_motor *motor);
void emergencyStop(t_motor *motor);
void turnLeft(t_motor *motor, unsigned seconds, unsigned angle, unsigned speed);
void turnRight(t_motor *motor, unsigned seconds, unsigned angle, unsigned speed);
void rotateLeft(t_motor *motor, unsigned seconds, unsigned speed);
void rotateRight(t_motor *motor, unsigned seconds, unsigned speed);

#endif
