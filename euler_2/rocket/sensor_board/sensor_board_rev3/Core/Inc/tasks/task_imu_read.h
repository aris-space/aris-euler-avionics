/*
 * task_imu_read.h
 *
 *  Created on: Nov 3, 2019
 *      Author: Jonas
 */

#ifndef INC_TASK_IMU_READ_H_
#define INC_TASK_IMU_READ_H_

/* Includes */
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "util.h"

/* Constants */
#define IMU20601_SAMPLING_FREQ SAMPLING_FREQ

extern osMessageQueueId_t preprocess_queue_imu_1;
extern osMessageQueueId_t preprocess_queue_imu_2Handle;

#endif /* INC_TASK_IMU_READ_H_ */
