#ifndef GYRO_H
#define GYRO_H

#include <bno055.h>

double getEulPitch(t_bno055 *bno055);
double getEulRoll(t_bno055 *bno055);
double getEulHead(t_bno055 *bno055);
double getQuaW(t_bno055 *bno055);
double getQuaX(t_bno055 *bno055);
double getQuaY(t_bno055 *bno055);
double getQuaZ(t_bno055 *bno055);
double getGravityX(t_bno055 *bno055);
double getGravityY(t_bno055 *bno055);
double getGravityZ(t_bno055 *bno055);
double getCalEulHead(t_bno055 *bno055);
double getCalEulRoll(t_bno055 *bno055);
double getCalEulPitch(t_bno055 *bno055);
#endif
