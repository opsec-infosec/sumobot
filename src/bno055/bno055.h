#ifndef BNO055_H
#define BNO055_H

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define	DEBUG

#define BNO055_ID            0xA0
#define POWER_MODE_NORMAL    0x00
//#define CALIB_BYTECOUNT      22
#define CALIB_BYTECOUNT      34
#define REGISTERMAP_END      0x7F

/* ------------------------------------------------------------ *
 * Page-0 registers with general confguration and data output   *
 * ------------------------------------------------------------ */
#define BNO055_CHIP_ID_ADDR  0x00

/* Page ID register, for page switching */
#define BNO055_PAGE_ID_ADDR  0x07

/* Accel data register */
#define BNO055_ACC_DATA_X_LSB_ADDR   0x08
#define BNO055_ACC_DATA_X_MSB_ADDR   0x09
#define BNO055_ACC_DATA_Y_LSB_ADDR   0x0A
#define BNO055_ACC_DATA_Y_MSB_ADDR   0x0B
#define BNO055_ACC_DATA_Z_LSB_ADDR   0x0C
#define BNO055_ACC_DATA_Z_MSB_ADDR   0x0D

/* Mag data register */
#define BNO055_MAG_DATA_X_LSB_ADDR   0x0E
#define BNO055_MAG_DATA_X_MSB_ADDR   0x0F
#define BNO055_MAG_DATA_Y_LSB_ADDR   0x10
#define BNO055_MAG_DATA_Y_MSB_ADDR   0x11
#define BNO055_MAG_DATA_Z_LSB_ADDR   0x12
#define BNO055_MAG_DATA_Z_MSB_ADDR   0x13

/* Gyro data registers */
#define BNO055_GYRO_DATA_X_LSB_ADDR  0x14
#define BNO055_GYRO_DATA_X_MSB_ADDR  0x15
#define BNO055_GYRO_DATA_Y_LSB_ADDR  0x16
#define BNO055_GYRO_DATA_Y_MSB_ADDR  0x17
#define BNO055_GYRO_DATA_Z_LSB_ADDR  0x18
#define BNO055_GYRO_DATA_Z_MSB_ADDR  0x19

/* Euler data registers */
#define BNO055_EULER_H_LSB_ADDR      0x1A
#define BNO055_EULER_H_MSB_ADDR      0x1B
#define BNO055_EULER_R_LSB_ADDR      0x1C
#define BNO055_EULER_R_MSB_ADDR      0x1D
#define BNO055_EULER_P_LSB_ADDR      0x1E
#define BNO055_EULER_P_MSB_ADDR      0x1F

/* Quaternion data registers */
#define BNO055_QUATERNION_DATA_W_LSB_ADDR 0x20
#define BNO055_QUATERNION_DATA_W_MSB_ADDR 0x21
#define BNO055_QUATERNION_DATA_X_LSB_ADDR 0x22
#define BNO055_QUATERNION_DATA_X_MSB_ADDR 0x23
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR 0x24
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR 0x25
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR 0x26
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR 0x27

/* Linear acceleration data registers */
#define BNO055_LIN_ACC_DATA_X_LSB_ADDR    0x28
#define BNO055_LIN_ACC_DATA_X_MSB_ADDR    0x29
#define BNO055_LIN_ACC_DATA_Y_LSB_ADDR    0x2A
#define BNO055_LIN_ACC_DATA_Y_MSB_ADDR    0x2B
#define BNO055_LIN_ACC_DATA_Z_LSB_ADDR    0x2C
#define BNO055_LIN_ACC_DATA_Z_MSB_ADDR    0x2D

/* Gravity data registers */
#define BNO055_GRAVITY_DATA_X_LSB_ADDR    0x2E
#define BNO055_GRAVITY_DATA_X_MSB_ADDR    0x2F
#define BNO055_GRAVITY_DATA_Y_LSB_ADDR    0x30
#define BNO055_GRAVITY_DATA_Y_MSB_ADDR    0x31
#define BNO055_GRAVITY_DATA_Z_LSB_ADDR    0x32
#define BNO055_GRAVITY_DATA_Z_MSB_ADDR    0x33

/* Temperature data register */
#define BNO055_TEMP_ADDR                  0x34

/* Status registers */
#define BNO055_CALIB_STAT_ADDR            0x35
#define BNO055_SELFTSTRES_ADDR            0x36
#define BNO055_INTR_STAT_ADDR             0x37

#define BNO055_SYS_CLK_STAT_ADDR          0x38
#define BNO055_SYS_STAT_ADDR              0x39
#define BNO055_SYS_ERR_ADDR               0x3A

/* Unit selection register */
#define BNO055_UNIT_SEL_ADDR              0x3B
#define BNO055_DATA_SELECT_ADDR           0x3C

/* Mode registers */
#define BNO055_OPR_MODE_ADDR              0x3D
#define BNO055_PWR_MODE_ADDR              0x3E

#define BNO055_SYS_TRIGGER_ADDR           0x3F
#define BNO055_TEMP_SOURCE_ADDR           0x40

/* Axis remap registers */
#define BNO055_AXIS_MAP_CONFIG_ADDR       0x41
#define BNO055_AXIS_MAP_SIGN_ADDR         0x42

/* Soft Iron Calibration registers */
#define BNO055_SIC_MATRIX_0_LSB_ADDR      0x43
#define BNO055_SIC_MATRIX_0_MSB_ADDR      0x44
#define BNO055_SIC_MATRIX_1_LSB_ADDR      0x45
#define BNO055_SIC_MATRIX_1_MSB_ADDR      0x46
#define BNO055_SIC_MATRIX_2_LSB_ADDR      0x47
#define BNO055_SIC_MATRIX_2_MSB_ADDR      0x48
#define BNO055_SIC_MATRIX_3_LSB_ADDR      0x49
#define BNO055_SIC_MATRIX_3_MSB_ADDR      0x4A
#define BNO055_SIC_MATRIX_4_LSB_ADDR      0x4B
#define BNO055_SIC_MATRIX_4_MSB_ADDR      0x4C
#define BNO055_SIC_MATRIX_5_LSB_ADDR      0x4D
#define BNO055_SIC_MATRIX_5_MSB_ADDR      0x4E
#define BNO055_SIC_MATRIX_6_LSB_ADDR      0x4F
#define BNO055_SIC_MATRIX_6_MSB_ADDR      0x50
#define BNO055_SIC_MATRIX_7_LSB_ADDR      0x51
#define BNO055_SIC_MATRIX_7_MSB_ADDR      0x52
#define BNO055_SIC_MATRIX_8_LSB_ADDR      0x53
#define BNO055_SIC_MATRIX_8_MSB_ADDR      0x54

/* Accelerometer Offset registers */
#define ACC_OFFSET_X_LSB_ADDR             0x55
#define ACC_OFFSET_X_MSB_ADDR             0x56
#define ACC_OFFSET_Y_LSB_ADDR             0x57
#define ACC_OFFSET_Y_MSB_ADDR             0x58
#define ACC_OFFSET_Z_LSB_ADDR             0x59
#define ACC_OFFSET_Z_MSB_ADDR             0x5A

/* Magnetometer Offset registers */
#define MAG_OFFSET_X_LSB_ADDR             0x5B
#define MAG_OFFSET_X_MSB_ADDR             0x5C
#define MAG_OFFSET_Y_LSB_ADDR             0x5D
#define MAG_OFFSET_Y_MSB_ADDR             0x5E
#define MAG_OFFSET_Z_LSB_ADDR             0x5F
#define MAG_OFFSET_Z_MSB_ADDR             0x60

/* Gyroscope Offset register s*/
#define GYRO_OFFSET_X_LSB_ADDR            0x61
#define GYRO_OFFSET_X_MSB_ADDR            0x62
#define GYRO_OFFSET_Y_LSB_ADDR            0x63
#define GYRO_OFFSET_Y_MSB_ADDR            0x64
#define GYRO_OFFSET_Z_LSB_ADDR            0x65
#define GYRO_OFFSET_Z_MSB_ADDR            0x66

/* Radius registers */
#define ACCEL_RADIUS_LSB_ADDR             0x67
#define ACCEL_RADIUS_MSB_ADDR             0x68
#define MAG_RADIUS_LSB_ADDR               0x69
#define MAG_RADIUS_MSB_ADDR               0x6A

/* ------------------------------------------------------------ *
 * Page-1 contains sensor component specific confguration data  *
 * ------------------------------------------------------------ */
#define BNO055_ACC_CONFIG_ADDR            0x08
#define BNO055_MAG_CONFIG_ADDR            0x09
#define BNO055_GYR_CONFIG0_ADDR           0x0A
#define BNO055_GYR_CONFIG1_ADDR           0x0B
#define BNO055_ACC_SLEEP_CONFIG_ADDR      0x0C
#define BNO055_GYR_SLEEP_CONFIG_ADDR      0x0D

/* ------------------------------------------------------------ *
 * BNO055 versions, status data and other infos struct          *
 * ------------------------------------------------------------ */
typedef struct s_bnoinf{
	char chip_id;  // reg 0x00 default 0xA0
	char acc_id;   // reg 0x01 default 0xFB
	char mag_id;   // reg 0x02 default 0x32
	char gyr_id;   // reg 0x03 default 0x0F
	char sw_lsb;   // reg 0x04 default 0x08
	char sw_msb;   // reg 0x05 default 0x03
	char bl_rev;   // reg 0x06 no default
	char opr_mode; // reg 0x3D default 0x1C
	char pwr_mode; // reg 0x3E default 0x00
	char axr_conf; // reg 0x41 default 0x24
	char axr_sign; // reg 0x42 default 0x00
	char sys_stat; // reg 0x39 system error status, range 0-6
	char selftest; // reg 0x36 self test result
	char sys_err;  // reg 0x3a system error code, 0=OK
	char unitsel;  // reg 0x3b SI units definition
	char temp_val; // reg 0x34 sensor temperature value
}t_bnoinf;

/* ------------------------------------------------------------ *
 * BNO055 calibration data struct. The offset ranges depend on  *
 * the component operation range. For example, the accelerometer*
 * range can be set as 2G, 4G, 8G, and 16G. I.e. the offset for *
 * the accelerometer at 16G has a range of +/- 16000mG. Offset  *
 * is stored on the sensor in two bytes with max value of 32768.*
 * ------------------------------------------------------------ */
typedef struct s_bnocal{
	char scal_st;  // reg 0x35 system calibration state, range 0-3
	char gcal_st;  // gyroscope calibration state, range 0-3
	char acal_st;  // accelerometer calibration state, range 0-3
	char mcal_st;  // magnetometer calibration state, range 0-3
	int  aoff_x;   // accelerometer offset, X-axis
	int  aoff_y;   // accelerometer offset, Y-axis
	int  aoff_z;   // accelerometer offset, Z-axis
	int  moff_x;   // magnetometer offset, X-axis
	int  moff_y;   // magnetometer offset, Y-axis
	int  moff_z;   // magnetometer offset, Z-axis
	int  goff_x;   // gyroscope offset, X-axis
	int  goff_y;   // gyroscope offset, Y-axis
	int  goff_z;   // gyroscope offset, Z-axis
	int acc_rad;   // accelerometer radius
	int mag_rad;   // magnetometer radius
}t_bnocal;

/* ------------------------------------------------------------ *
 * BNO055 measurement data structs. Data gets filled in based   *
 * on the sensor component type that was requested for reading. *
 * ------------------------------------------------------------ */
typedef struct s_bnoacc{
	double adata_x;   // accelerometer data, X-axis
	double adata_y;   // accelerometer data, Y-axis
	double adata_z;   // accelerometer data, Z-axis
}t_bnoacc;

typedef struct s_bnomag{
	double mdata_x;   // magnetometer data, X-axis
	double mdata_y;   // magnetometer data, Y-axis
	double mdata_z;   // magnetometer data, Z-axis
}t_bnomag;

typedef struct s_bnogyr{
	double gdata_x;   // gyroscope data, X-axis
	double gdata_y;   // gyroscope data, Y-axis
	double gdata_z;   // gyroscope data, Z-axis
}t_bnogyr;

typedef struct s_bnoeul{
	double eulHead;  // Euler heading data
	double eulRoll;  // Euler roll data
	double eulPitch;  // Euler picth data
	pthread_mutex_t *m_bnoeul;	// Euler Multex Array
	pthread_t pth_bnoeul;	// Euler Thread
}t_bnoeul;

typedef struct s_caleul {
	double head;
	double roll;
	double pitch;
	pthread_mutex_t m_caleul;
}t_caleul;
typedef struct s_bnoqua{
	double quaterW;  // Quaternation data W
	double quaterX;  // Quaternation data X
	double quaterY;  // Quaternation data Y
	double quaterZ;  // Quaternation data Z
	pthread_mutex_t *m_bnoqua;	// Quaternation Mutex Array
	pthread_t pth_bnoqua;	// Quaternation Thread
}t_bnoqua;

typedef struct s_bnogra{
	double gravityX;  // Gravity Vector X
	double gravityY;  // Gravity Vector Y
	double gravityZ;  // Gravity Vector Z
	pthread_mutex_t *m_bnogra;
	pthread_t pth_bnogra;
}t_bnogra;

typedef struct s_bnolin{
	double linacc_x;  // Linear Acceleration X
	double linacc_y;  // Linear Acceleration Y
	double linacc_z;  // Linear Acceleration Z
}t_bnolin;

/* ------------------------------------------------------------ *
 * BNO055 accelerometer gyroscope magnetometer config structs   *
 * ------------------------------------------------------------ */
typedef struct s_bnoaconf{
	int pwrmode;      // p-1 reg 0x08 accelerometer power mode
	int bandwth;      // p-1 reg 0x08 accelerometer bandwidth
	int range;        // p-1 reg 0x08 accelerometer rate
	int slpmode;      // p-1 reg 0x0C accelerometer sleep mode
	int slpdur;       // p-1 reg 0x0C accelerometer sleep duration
}t_bnoaconf;

typedef struct s_bnomconf{
	int pwrmode;      // p-1 reg 0x09 magnetometer power mode
	int oprmode;      // p-1 reg 0x09 magnetometer operation
	int outrate;      // p-1 reg 0x09 magnetometer output rate
}t_bnomconf;

typedef struct s_bnogconf{
	int pwrmode;      // p-1 reg 0x0B gyroscope power mode
	int bandwth;      // p-1 reg 0x0A gyroscope bandwidth
	int range;        // p-1 reg 0x0A gyroscope range
	int slpdur;       // p-1 reg 0x0D gyroscope sleep duration
	int aslpdur;      // p-1 reg 0x0D gyroscope auto sleep dur
}t_bnogconf;

/* ------------------------------------------------------------ *
 * Operations and power mode, name to value translation         *
 * ------------------------------------------------------------ */
typedef enum {
	config   = 0x00,
	acconly  = 0x01,
	magonly  = 0x02,
	gyronly  = 0x03,
	accmag   = 0x04,
	accgyro  = 0x05,
	maggyro  = 0x06,
	amg      = 0x07,
	imu      = 0x08,
	compass  = 0x09,
	m4g      = 0x0A,
	ndof     = 0x0B,
	ndof_fmc = 0x0C
} opmode_t;

typedef enum {
	normal  = 0x00,
	low     = 0x01,
	suspend = 0x02
} power_t;

typedef enum {
	uncal,
	minimal_cal,
	mostly_cal,
	fully_cal
} t_calstate;

typedef struct s_bno055 {
	int				fd;
	char			*i2cbus;
	int				addr;
	pthread_mutex_t	m_locki2cBus;
	opmode_t		mode;
	//t_bnoinf		*bnoinf;
	//t_bnomconf 	*bnomconf;
	//t_bnoacc		*bnoacc;
	//t_bnoaconf	*bnoaconf;
	//t_bnogyr		*bnogyr;
	//t_bnogconf	*bnogconf;
	//t_bnomag		*bnomag;
	//t_bnomconf	*bnomconf;
	//t_bnolin		*bnolin;
	t_bnogra		*bnogra;
	t_bnoeul		*bnoeul;
	t_bnoqua		*bnoqua;
	t_caleul		*caleul;
	//t_bnocal		*bnocal;
}t_bno055;


int setMode(t_bno055 *bno055, opmode_t newmode);
int getMode(t_bno055 *bno055);
int bnoReset(t_bno055 *bno055);
int getEul(t_bno055  *bno055);
int getQua(t_bno055 *bno055);
void getCalEul(t_bno055 *bno055);
int initGyro(t_bno055 *bno055);
int startGyro(t_bno055 *bno055);
void stopGyro(t_bno055 *bno055);


#endif
