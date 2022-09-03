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
		perror("Error failed to open I2C bus [%s].\n", bno055->i2cbus);
		syslog(LOG_DEBUG, "Error failed to open I2C bus (%s,%d)", __FILE__, __LINE__);
		return -1;
	}

	if(ioctl(fd, I2C_SLAVE, bno055->addr) != 0) {
		perror("Error can't find sensor at address [0x%02X].\n", bno055->addr);
		syslog(LOG_DEBUG, "Error can't find sensor at address (%s,%d)", __FILE__, __LINE__);
		return -1;
	}

	if(write(fd, &reg, 1) != 1) {
		perror("Error: I2C write failure register [0x%02X], sensor addr [0x%02X]?\n", reg, bno055->addr);
		syslog(LOG_DEBUG, "Error: I2C write failure register [0x%02X], sensor addr (%s,%d)", __FILE__, __LINE__);
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
// Allocate Memory for Gyro
//
static int allocGyro(t_bno055 *bno055) {
	bno055->bnoeul = (t_bnoeul *)malloc(sizeof(t_bnoeul));
	bno055->bnoqua = (t_bnoqua *)malloc(sizeof(t_bnoqua));
	bno055->i2cbus = (char *)malloc(sizeof(char) * 256);
	if (!bno055->bnoeul || !bno055->bnoqua || !bno055->i2cbus) {
		perror ("Failed to malloc bno055 structure\n");
    	syslog(LOG_DEBUG, "ERROR: BNO055 Malloc Failure (%s,%d)", __FILE__, __LINE__);
		freeGyro(bno055);
		return EXIT_FAILURE;
	}
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

		if (write(bno055->fd, data, 2) != 2) {
			perror("Error: I2C write failure for register\n");
			return EXIT_FAILURE;
		}

		// delay 7ms + 3ms safety margin during switching mode
	  	usleep(10 * 1000);
    }
	data[1] = newmode;
	if (write(bno055->fd, data, 2) != 2) {
		perror("Error: I2C write failure for register\n");
		return EXIT_FAILURE;
	}
	usleep(25 * 1000);
	bno055->mode = getMode(bno055);
	return EXIT_SUCCESS;
}

//
// get_mode() - returns sensor operational mode register 0x3D
// Reads 1 byte from Operations Mode register 0x3d, and use
// only the lowest 4 bit. Bits 4-7 are unused and masked
//
int getMode(t_bno055 *bno055) {
   	int reg = BNO055_OPR_MODE_ADDR;
    unsigned int data = 0;

	if ((write(bno055->fd, &reg, 1)) != 1) {
      perror("Error: I2C write failure for register\n");
	  return -1;
	}

	if ((read(bno055->fd, &data, 1) != 1)) {
		perror("Error: I2C write failure for register\n");
	 	return -1;
	}
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

	if (write(bno055->fd, data, 2) != 2) {
		perror("Error: I2C write failure for register\n");
	  	return EXIT_FAILURE;
	}

	// Reboot take 650ms to boot up
 	usleep(650 * 1000);
   	return EXIT_SUCCESS;
}

//
// Euler Angles
//  get_eul() - read Euler orientation into the global struct
//	3 Words = Heading, Roll, Pitch
//	Mode = > 0x08
//
int getEul(t_bno055 *bno055) {
	char reg = BNO055_EULER_H_LSB_ADDR;
	unsigned char data[6] = {0, 0, 0, 0, 0, 0};
	int16_t buffer;

	if (bno055->mode < imu) {
		perror("Mode has to be > than 0x08\n");
		return EXIT_FAILURE;
	}

	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register\n");
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, data, 6) != 6) {
		perror("Error: I2C read failure for register data\n");
		return EXIT_FAILURE;
	}

	buffer = ((int16_t)data[1] << 8) | data[0];
	bno055->bnoeul->eul_head = (double)buffer / 16.0;

	buffer = ((int16_t)data[3] << 8) | data[2];
	bno055->bnoeul->eul_roll = (double)buffer / 16.0;

	buffer = ((int16_t)data[5] << 8) | data[4];
	bno055->bnoeul->eul_pitc = (double)buffer / 16.0;

	#ifdef DEBUG
		printf("HEAD: %f, ROLL: %f, PITCH: %f\n", bno055->bnoeul->eul_head
												, bno055->bnoeul->eul_roll
												, bno055->bnoeul->eul_pitc);
	#endif
	return EXIT_SUCCESS;
}

//
//  Quaternion
//  get_qua() - read Quaternation data into the global struct
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

	if(write(bno055->fd, &reg, 1) != 1) {
		perror("Error: I2C write failure for register\n");
		return EXIT_FAILURE;
	}

	if(read(bno055->fd, data, 8) != 8) {
		perror("Error: I2C read failure for register data\n");
		return EXIT_FAILURE;
	}

	buffer = ((int16_t)data[1] << 8) | data[0];
	bno055->bnoqua->quater_w = (double) buffer / 16384.0;

	buffer = ((int16_t)data[3] << 8) | data[2];
	bno055->bnoqua->quater_x = (double) buffer / 16384.0;

	buffer = ((int16_t)data[5] << 8) | data[4];
	bno055->bnoqua->quater_y = (double) buffer / 16384.0;

	buffer = ((int16_t)data[7] << 8) | data[6];
	bno055->bnoqua->quater_z = (double) buffer / 16384.0;

	#ifdef DEBUG
		printf("W: %f, X: %f, Y: %f, Z: %f\n", bno055->bnoqua->quater_w
											 , bno055->bnoqua->quater_x
											 , bno055->bnoqua->quater_y
											 , bno055->bnoqua->quater_z);
	#endif
	return EXIT_SUCCESS;
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
		printf("INITIAL MODE = %d\n", getMode(bno055));
		printf("SET MODE TO 9DOF\n");
	#endif

	mode = setMode(bno055, ndof);

	#ifdef DEBUG
		printf("NEW MODE = %d\n", mode);
	#endif
	return EXIT_SUCCESS;
}

//
// Free Gyro BNO055 Resources
//
void freeGyro(t_bno055 *bno055) {
	if (closei2c(bno055->fd) != 0)
		perror("Failed to close i2c bus\n");
	bno055->fd = -1;

	if (bno055->bnoeul) {
		free(bno055->bnoeul);
		bno055->bnoeul = NULL;
	}

	if (bno055->bnoqua) {
		free(bno055->bnoqua);
		bno055->bnoqua = NULL;
	}

	if (bno055->i2cbus) {
		free(bno055->i2cbus);
		bno055->i2cbus = NULL;
	}
}
