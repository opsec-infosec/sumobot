
#include "./bno055/bno055.h"
#include "./includes/gyro.h"

double getEulHead(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[0]);
	result = bno055->bnoeul->eulHead;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[0]);
	return result;
}

double getEulRoll(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[1]);
	result = bno055->bnoeul->eulRoll;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[1]);
	return result;
}

double getEulPitch(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[2]);
	result = bno055->bnoeul->eulPitch;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[2]);
	return result;
}

double getCalEulHead(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->caleul->m_caleul);
	result = bno055->caleul->head;
	pthread_mutex_unlock(&bno055->caleul->m_caleul);
	return result;
}

double getCalEulRoll(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->caleul->m_caleul);
	result = bno055->caleul->roll;
	pthread_mutex_unlock(&bno055->caleul->m_caleul);
	return result;
}

double getCalEulPitch(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->caleul->m_caleul);
	result = bno055->caleul->pitch;
	pthread_mutex_unlock(&bno055->caleul->m_caleul);
	return result;
}

double getQuaW(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[0]);
	result = bno055->bnoqua->quaterW;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[0]);
	return result;
}
double getQuaX(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[1]);
	result = bno055->bnoqua->quaterX;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[1]);
	return result;
}
double getQuaY(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[2]);
	result = bno055->bnoqua->quaterY;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[2]);
	return result;
}
double getQuaZ(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[3]);
	result = bno055->bnoqua->quaterZ;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[3]);
	return result;
}

double getGravityX(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnogra->m_bnogra[0]);
	result = bno055->bnogra->gravityX;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[0]);
	return result;
}

double getGravityY(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnogra->m_bnogra[1]);
	result = bno055->bnogra->gravityY;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[1]);
	return result;
}

double getGravityZ(t_bno055 *bno055) {
	double result = 0;

	pthread_mutex_lock(&bno055->bnogra->m_bnogra[2]);
	result = bno055->bnogra->gravityZ;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[2]);
	return result;
}
