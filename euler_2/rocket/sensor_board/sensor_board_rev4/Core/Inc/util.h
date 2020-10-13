/*
 * util.h
 *
 *  Created on: Feb 24, 2020
 *      Author: stoja
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "cmsis_os.h"

/* OVERALL SAMPLING FREQUENCY */
#define SAMPLING_FREQ 100


/* Constants */
#define PREPROCESS_QUEUE_SIZE 128
#define BARO_MUTEX_TIMEOUT 0
#define IMU_MUTEX_TIMEOUT 0

/** BASIC TYPES **/

/* Timestamp */
typedef uint32_t timestamp_t;

/** SENSOR DATA TYPES **/

/* IMU data */
typedef struct {
	int16_t gyro_x, gyro_y, gyro_z;
	int16_t acc_x, acc_y, acc_z;
	timestamp_t ts;
} imu_data_t;

/* Barometer data */
typedef struct {
	int32_t pressure;
	int32_t temperature;
	timestamp_t ts;
} baro_data_t;

typedef struct {
	baro_data_t baro;
	imu_data_t imu_1;
	imu_data_t imu_2;
	uint8_t checksum;
} sb_data_t;

static const imu_data_t EMPTY_IMU = { 0 };

/** DEBUGGING **/
/* Read In Fake Sensor Data */
#define USB_DATA_ENABLE 0

/* Debug flag */
#ifdef DEBUG
#undef DEBUG
#endif
/* Comment the next line in order to disable debug mode */
#define DEBUG

#ifdef DEBUG
osMutexId_t print_mutex;
#define PRINT_BUFFER_LEN 200
char print_buffer[PRINT_BUFFER_LEN];
#endif

uint8_t UsbPrint(const char *format, ...);

#endif /* INC_UTIL_H_ */
