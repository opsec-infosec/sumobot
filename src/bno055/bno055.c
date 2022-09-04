#include "bno055.h"

#define BNO055_ADDR	0x28
#define I2CBUS	"/dev/i2c-1"

//
// Opens i2c Bus at address addr and
// returns a file descriptor
//
static int openi2c(t_bno055 *bno055) {
	int fd;
   	char reg = BNO055_CHIP_ID_ADDR;

	bno055->addr = BNO055_ADDR;
	bno055->i2cbus = I2CBUS;

	fd = open(bno055->i2cbus, O_RDWR);
	if (fd < 0) {
		perror("Error failed to open I2C bus\n");
		syslog(LOG_DEBUG, "Error failed to open I2C bus (%s,%d)", __FILE__, __LINE__);
		return -1;
	}

	if(ioctl(fd, I2C_SLAVE, bno055->addr) != 0) {
		perror("Error can't find sensor at address\n");
		syslog(LOG_DEBUG, "Error can't find sensor at address (%s,%d)", __FILE__, __LINE__);
		return -1;
	}

	if(write(fd, &reg, 1) != 1) {
		perror("Error: I2C write failure register\n");
		syslog(LOG_DEBUG, "Error: I2C write failure register and sensor addr (%s,%d)", __FILE__, __LINE__);
		return -1;
	}
	bno055->fd = fd;
	return fd;
}

//
// Close i2c Bus
//
static int closei2c(int fd) {
	if (fd >= 0) {
		return close(fd);
	}
	perror ("Failed to close i2c Bus, invalud fd\n");
    syslog(LOG_DEBUG, "ERROR: i2c bus failure (%s,%d)", __FILE__, __LINE__);
	return EXIT_FAILURE;
}

//
// Free Gyro BNO055 Resources
//
static void freeGyro(t_bno055 *bno055) {
	if (closei2c(bno055->fd) != 0)
		perror("Failed to close i2c bus\n");
	bno055->fd = -1;

	if (bno055->caleul) {
		free(bno055->caleul);
		bno055->caleul = NULL;
	}
	for (int i = 0; i < 3; i++)
		pthread_mutex_destroy(&bno055->bnoeul->m_bnoeul[i]);
	if (bno055->bnoeul->m_bnoeul) {
		free(bno055->bnoeul->m_bnoeul);
		bno055->bnoeul->m_bnoeul = NULL;
	}
	if (bno055->bnoeul) {
		free(bno055->bnoeul);
		bno055->bnoeul = NULL;
	}

	for (int i = 0; i < 4; i++)
		pthread_mutex_destroy(&bno055->bnoqua->m_bnoqua[i]);
	if (bno055->bnoqua->m_bnoqua) {
		free(bno055->bnoqua->m_bnoqua);
		bno055->bnoqua->m_bnoqua = NULL;
	}
	if (bno055->bnoqua) {
		free(bno055->bnoqua);
		bno055->bnoqua = NULL;
	}

	for (int i = 0; i < 3; i++) {
		pthread_mutex_destroy(&bno055->bnogra->m_bnogra[i]);
	}
	if (bno055->bnogra->m_bnogra) {
		free(bno055->bnogra->m_bnogra);
		bno055->bnogra->m_bnogra = NULL;
	}
	if (bno055->bnogra) {
		free(bno055->bnogra);
		bno055->bnogra = NULL;
	}

	if (bno055->i2cbus) {
		free(bno055->i2cbus);
		bno055->i2cbus = NULL;
	}

	pthread_mutex_destroy(&bno055->caleul->m_caleul);
	pthread_mutex_destroy(&bno055->m_locki2cBus);
}

//
// Allocate Memory for Gyro
// Init mutexes
//
static int allocGyro(t_bno055 *bno055) {
	bno055->bnoeul = (t_bnoeul *)malloc(sizeof(t_bnoeul));
	bno055->bnoqua = (t_bnoqua *)malloc(sizeof(t_bnoqua));
	bno055->bnogra = (t_bnogra *)malloc(sizeof(t_bnogra));
	bno055->caleul = (t_caleul *)malloc(sizeof(t_caleul));
	bno055->i2cbus = (char *)malloc(sizeof(char) * 256);
	bno055->bnoeul->m_bnoeul = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * 3);
	bno055->bnoqua->m_bnoqua = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * 4);
	bno055->bnogra->m_bnogra = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * 3);
	if (!bno055->bnoeul || !bno055->bnoqua || !bno055->bnogra || !bno055->i2cbus ||
		!bno055->bnoeul->m_bnoeul || !bno055->bnoqua->m_bnoqua || !bno055->bnogra->m_bnogra ||
		!bno055->caleul) {
		perror ("Failed to malloc bno055 structure\n");
    	syslog(LOG_DEBUG, "ERROR: BNO055 Malloc Failure (%s,%d)", __FILE__, __LINE__);
		freeGyro(bno055);
		return EXIT_FAILURE;
	}

	// init mutexes
	for (int i = 0; i < 3; i++) {
		pthread_mutex_init(&bno055->bnoeul->m_bnoeul[i], NULL);
		pthread_mutex_init(&bno055->bnogra->m_bnogra[i], NULL);
	}
	for (int i =0; i < 4; i++)
		pthread_mutex_init(&bno055->bnoqua->m_bnoqua[i], NULL);
	pthread_mutex_init(&bno055->caleul->m_caleul, NULL);
	pthread_mutex_init(&bno055->m_locki2cBus, NULL);

	return EXIT_SUCCESS;
}

//
// Set Mode (ie fusion, imu, non-fusion etc...)
// set_mode() - set the sensor operational mode register 0x3D
// The modes cannot be switched over directly, first it needs
// to be set to "config" mode before switching to the new mode.
//
int setMode(t_bno055 *bno055, opmode_t newmode) {
	char data[2] = {BNO055_OPR_MODE_ADDR, config};
   	opmode_t oldmode = getMode(bno055);

	if(oldmode == newmode)
		return(0); // if new mode is the same
	else if(oldmode > 0 && newmode > 0) {  // switch to "config" first
		pthread_mutex_lock(&bno055->m_locki2cBus);
		if (write(bno055->fd, data, 2) != 2) {
			perror("Error: I2C write failure for register setMode()\n");
			pthread_mutex_unlock(&bno055->m_locki2cBus);
			return EXIT_FAILURE;
		}
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		// delay 7ms + 3ms safety margin during switching mode
	  	usleep(10 * 1000);
    }

	data[1] = newmode;
	pthread_mutex_lock(&bno055->m_locki2cBus);
	if (write(bno055->fd, data, 2) != 2) {
		perror("Error: I2C write failure for register setMode()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);
	usleep(25 * 1000);
	bno055->mode = getMode(bno055);
	return bno055->mode;
}

//
// get_mode() - returns sensor operational mode register 0x3D
// Reads 1 byte from Operations Mode register 0x3d, and use
// only the lowest 4 bit. Bits 4-7 are unused and masked
//
int getMode(t_bno055 *bno055) {
   	int reg = BNO055_OPR_MODE_ADDR;
    unsigned int data = 0;

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if ((write(bno055->fd, &reg, 1)) != 1) {
    	perror("Error: I2C write failure for register getMode()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
	  	return EXIT_FAILURE;
	}

	if ((read(bno055->fd, &data, 1) != 1)) {
		perror("Error: I2C write failure for register getMode()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
	 	return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	bno055->mode = data & 0x0F;
   	return data & 0x0F;  // only return the lowest 4 bits
}

//
// Soft Rest
// bno_reset() resets the sensor. It will come up in CONFIG mode.
//
int bnoReset(t_bno055 *bno055) {
	char data[2];
   	data[0] = BNO055_SYS_TRIGGER_ADDR;
   	data[1] = 0x20;

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if (write(bno055->fd, data, 2) != 2) {
		perror("Error: I2C write failure for register bnoReset()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
	  	return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	// Reboot take 650ms to boot up
 	usleep(650 * 1000);
   	return EXIT_SUCCESS;
}

//
// Euler Angles
//  getEul() - read Euler orientation into the global struct
//	3 Words = Heading, Roll, Pitch
//	Mode = > 0x08
//
int getEul(t_bno055 *bno055) {
	char reg = BNO055_EULER_H_LSB_ADDR;
	unsigned char data[6] = {0, 0, 0, 0, 0, 0};
	int16_t buffer;

	if (bno055->mode < imu) {
		perror("Mode has to be >= than 0x08\n");
		return EXIT_FAILURE;
	}

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register getEul()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, data, 6) != 6) {
		perror("Error: I2C read failure for register data getEul()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	buffer = ((int16_t)data[1] << 8) | data[0];
	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[0]);
	bno055->bnoeul->eulHead = (double)buffer / 16.0;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[0]);

	buffer = ((int16_t)data[3] << 8) | data[2];
	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[1]);
	bno055->bnoeul->eulRoll = (double)buffer / 16.0;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[1]);

	buffer = ((int16_t)data[5] << 8) | data[4];
	pthread_mutex_lock(&bno055->bnoeul->m_bnoeul[2]);
	bno055->bnoeul->eulPitch = (double)buffer / 16.0;
	pthread_mutex_unlock(&bno055->bnoeul->m_bnoeul[2]);

	return EXIT_SUCCESS;
}

//
//  Quaternion
//  getQua() - read Quaternation data into the global struct
//	4 Words = W, X, Y, Z
//	Mode = > 0x08
//
int getQua(t_bno055 *bno055) {
	char reg = BNO055_QUATERNION_DATA_W_LSB_ADDR;
	unsigned char data[8] = {0};
	int16_t buffer;

	if (bno055->mode < imu) {
		perror("Mode has to be > than 0x08\n");
		return EXIT_FAILURE;
	}

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register getQua()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, data, 8) != 8) {
		perror("Error: I2C read failure for register data getQya()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	buffer = ((int16_t)data[1] << 8) | data[0];
	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[0]);
	bno055->bnoqua->quaterW = (double) buffer / 16384.0;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[0]);

	buffer = ((int16_t)data[3] << 8) | data[2];
	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[1]);
	bno055->bnoqua->quaterX = (double) buffer / 16384.0;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[1]);

	buffer = ((int16_t)data[5] << 8) | data[4];
	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[2]);
	bno055->bnoqua->quaterY = (double) buffer / 16384.0;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[2]);

	buffer = ((int16_t)data[7] << 8) | data[6];
	pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[3]);
	bno055->bnoqua->quaterZ = (double) buffer / 16384.0;
	pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[3]);

	return EXIT_SUCCESS;
}

//
//  Gravety Vector
//  getGra() - read Quaternation data into the global struct
//	3 Words = X, Y, Z
//	Mode = > 0x08
//
int getGra(t_bno055 *bno055) {
	char reg = BNO055_UNIT_SEL_ADDR;
	unsigned char data[6] = {0, 0, 0, 0, 0, 0};
	int16_t buffer;
	char unitSel;
	double scaleFact;

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register getGra()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, &unitSel, 1) != 1) {
		perror("Error: I2C read failure for register data getGra()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	if((unitSel >> 0) & 0x01)
		scaleFact = 1.0;
	else
		scaleFact = 100.0;

	reg = BNO055_GRAVITY_DATA_X_LSB_ADDR;

	pthread_mutex_lock(&bno055->m_locki2cBus);
	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register getGra()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, data, 6) != 6) {
		perror("Error: I2C read failure for register data getGra()\n");
		pthread_mutex_unlock(&bno055->m_locki2cBus);
		return EXIT_FAILURE;
	}
	pthread_mutex_unlock(&bno055->m_locki2cBus);

	buffer = ((int16_t)data[1] << 8) | data[0];
	pthread_mutex_lock(&bno055->bnogra->m_bnogra[0]);
	bno055->bnogra->gravityX = (double) buffer / scaleFact;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[0]);

	buffer = ((int16_t)data[3] << 8) | data[2];
	pthread_mutex_lock(&bno055->bnogra->m_bnogra[1]);
	bno055->bnogra->gravityY = (double) buffer / scaleFact;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[1]);

	buffer = ((int16_t)data[5] << 8) | data[4];
	pthread_mutex_lock(&bno055->bnogra->m_bnogra[2]);
	bno055->bnogra->gravityZ = (double) buffer / scaleFact;
	pthread_mutex_unlock(&bno055->bnogra->m_bnogra[2]);

	return EXIT_SUCCESS;
}

void getCalEul(t_bno055 *bno055) {
	for (int i = 0; i < 4; i++) {
		pthread_mutex_lock(&bno055->bnoqua->m_bnoqua[i]);
	}
	double w = bno055->bnoqua->quaterW;
	double x = bno055->bnoqua->quaterX;
	double y = bno055->bnoqua->quaterY;
	double z = bno055->bnoqua->quaterZ;
	for (int i = 0; i < 4; i++) {
		pthread_mutex_unlock(&bno055->bnoqua->m_bnoqua[i]);
	}

	pthread_mutex_lock(&bno055->caleul->m_caleul);

	// Pitch Axis (Y)
	double sinRollcosPitch = 2 * (w * x + y * z);
	double cosRollcosPitch = 1 - 2 * (x * x + y * y);
	bno055->caleul->pitch = (atan2(sinRollcosPitch, cosRollcosPitch)) * (180 / M_PI);

	// Roll Axis (X)
	double sinPitch = 2 * (w * y - z * x);
	if (fabs(sinPitch) >= 1.0f)
		bno055->caleul->roll = (copysign(M_PI_2, sinPitch)) * (180 / M_PI);
	else
		bno055->caleul->roll = (asin(sinPitch)) * (180 / M_PI);

	// Yaw Axis (Heading)(Z)
	double sinHeadcosPitch = 2 * (w * z + x * y);
	double cosHeadcosPitch = 1 - 2 * (y * y + z * z);
	bno055->caleul->head = (atan2(sinHeadcosPitch, cosHeadcosPitch)) * (180 / M_PI);

	pthread_mutex_unlock(&bno055->caleul->m_caleul);
}

//
// Euler Angles Thread
// 100Hz Fusion Update Rate
//
static void *eulerThread(void *bno055) {
	if (((t_bno055 *)bno055)->mode >= imu) {
		while (1) {
			getEul((t_bno055 *)bno055);
			usleep(15000);
		}
	}
	return NULL;
}

//
// Quaternation Angles Thread
// 100Hz Fusion Update Rate
//
static void *quaternationThread(void *bno055) {
	if (((t_bno055 *)bno055)->mode >= imu) {
		while(1) {
			getQua((t_bno055 *)bno055);
			getCalEul((t_bno055 *)bno055);
			usleep(15000);
		}
	}
	return NULL;
}

//
// Gravity Vector Thread
// 100Hz Fusion Update Rate
//
static void *gravityThread(void *bno055) {
	if (((t_bno055 *)bno055)->mode >= imu) {
		while (1) {
			getGra((t_bno055 *)bno055);
			usleep(15000);
		}
	}
	return NULL;
}

//
// Start Gyro Acquisition Threads
// Be sure to initGyro First
//
int startGyro(t_bno055 *bno055) {
	if (pthread_create(&bno055->bnoeul->pth_bnoeul, NULL, &eulerThread, bno055)) {
		perror("Error: Gyro Euler Thread Creation Failed\n");
		syslog(LOG_DEBUG, "ERROR: Gyro Euler Thread Creation Failed (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	pthread_detach(bno055->bnoeul->pth_bnoeul);

	if (pthread_create(&bno055->bnoqua->pth_bnoqua, NULL, &quaternationThread, bno055)) {
		perror("Error: Gyro Quaternation Thread Creation Failed\n");
		syslog(LOG_DEBUG, "ERROR: Gyro Quaternation Thread Creation Failed (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	pthread_detach(bno055->bnoqua->pth_bnoqua);

	if (pthread_create(&bno055->bnogra->pth_bnogra, NULL, &gravityThread, bno055)) {
		perror("Error: Gyro Gravity Thread Creation Failed\n");
		syslog(LOG_DEBUG, "ERROR: Gyro Gravity Thread Creation Failed (%s,%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}
	pthread_detach(bno055->bnogra->pth_bnogra);

	return EXIT_SUCCESS;
}

//
// Stop Gyro Acquisition Threads
// Free Gyro Resources
//
void stopGyro(t_bno055 *bno055) {
	pthread_cancel(bno055->bnoeul->pth_bnoeul);
	pthread_cancel(bno055->bnoqua->pth_bnoqua);
	pthread_cancel(bno055->bnogra->pth_bnogra);
	freeGyro(bno055);
}

//
// Initialize Gyro BNO055
//  Allocate Memory and set Gyro Mode
//
int initGyro(t_bno055 *bno055) {
	int mode;

	if (allocGyro(bno055)) {
		return EXIT_FAILURE;
	}

	if (openi2c(bno055) < 0) {
		return EXIT_FAILURE;
	}

	// initial soft reset
	bnoReset(bno055);

	#ifdef DEBUG
		printf("I2c FD: %d\n", bno055->fd);
		printf("INITIAL MODE = %x\n", getMode(bno055));
		printf("SET MODE TO 9DOF\n");
	#endif

	mode = setMode(bno055, ndof);

	#ifdef DEBUG
		printf("NEW MODE = %x\n", mode);
	#endif
	// Delay 5 seconds
	usleep(5000000);
	return EXIT_SUCCESS;
}
