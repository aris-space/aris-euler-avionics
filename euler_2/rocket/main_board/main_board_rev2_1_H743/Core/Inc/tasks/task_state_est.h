/*
 * task_state_est.h
 *
 *  Created on: Nov 29, 2019
 *      Author: Jonas
 */

#ifndef INC_TASKS_TASK_STATE_EST_H_
#define INC_TASKS_TASK_STATE_EST_H_

/* Includes */
#include "util/mutex.h"
#include "util/util.h"
#include "../aris-euler-state-estimation/Inc/env.h"
/* Constants */
/* -> Are in state_est_settings */

/* Commands */

/* Extern */
/* Sensor Board 1 */
extern custom_mutex_t sb1_mutex;
extern imu_data_t sb1_imu_1;
extern imu_data_t sb1_imu_2;
extern baro_data_t sb1_baro;

/* Sensor Board 2 */
extern custom_mutex_t sb2_mutex;
extern imu_data_t sb2_imu_1;
extern imu_data_t sb2_imu_2;
extern baro_data_t sb2_baro;

/* Sensor Board 3 */
extern custom_mutex_t sb3_mutex;
extern imu_data_t sb3_imu_1;
extern imu_data_t sb3_imu_2;
extern baro_data_t sb3_baro;

/* State Estimation Mutex */
extern custom_mutex_t state_est_mutex;
extern state_est_data_t state_est_data_global;

/* fsm Mutex */
extern custom_mutex_t fsm_mutex;
extern custom_mutex_t environment_mutex;
extern flight_phase_detection_t global_flight_phase_detection;
extern env_t global_env;

/* Command Mutex */
extern custom_mutex_t command_mutex;
extern command_e global_telemetry_command;

/* Airbrake Extension Mutex */
extern custom_mutex_t airbrake_ext_mutex;
extern uint32_t global_airbrake_ext_meas;

/* Tasks */
void vTaskStateEst(void *argument);

#endif /* INC_TASKS_TASK_STATE_EST_H_ */
