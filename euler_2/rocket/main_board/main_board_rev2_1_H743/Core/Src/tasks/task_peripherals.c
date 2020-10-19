/*
 * task_peripherals.c
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */

#include "tasks/task_peripherals.h"
#include "main.h"

//void user_pwm_setvalue(uint16_t value);

void vTaskPeripherals(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  osDelay(1200);
  HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_SET);


  bool camera_enabled = false;
  int32_t camera_start_time = 0;

  /* buzzer variables */
  bool buzzer_on_fsm = false;
  bool buzzer_on_telemetry = false;
  uint8_t buzzercounter = 0;

  /* Telemetry Command */
  command_e telemetry_command = IDLE_COMMAND;

  /* Phase detection struct */
  flight_phase_detection_t flight_phase_detection = {0};
  reset_flight_phase_detection(&flight_phase_detection);

  /* Infinite loop */

  tick_update = osKernelGetTickFreq() / PERIPHERALS_SAMPLING_FREQ;
  tick_count = osKernelGetTickCount();

  while (1) {
    /* Tick Update */
    tick_count += tick_update;

    /* Read Telemetry Command */
    read_mutex(&command_mutex, &global_telemetry_command, &telemetry_command,
              sizeof(global_telemetry_command));

    /* Read Flight Phase */
    read_mutex(&fsm_mutex, &global_flight_phase_detection,
              &flight_phase_detection, sizeof(global_flight_phase_detection));

    /* Enable Buzzer trough Telemetry */
    if (telemetry_command == ENABLE_BUZZER) {
      buzzer_on_telemetry = !buzzer_on_telemetry;
    }

    /* Enable Buzzer Trough FSM */
    if (flight_phase_detection.flight_phase == TOUCHDOWN) {
      buzzer_on_fsm = true;
    }

    /* Enable Self Power Hold */
    if (telemetry_command == TELEMETRY_HIGH_SAMPLING) {
      HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_RESET);
    }

    /* Disable Self Power Hold */
    if (telemetry_command == DISABLE_SELF_HOLD) {
      HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_SET);
    }

    if(telemetry_command == DISABLE_CAMERA){
    	camera_enabled = false;
    }

    if ((telemetry_command == ENABLE_CAMERA) | (flight_phase_detection.flight_phase == THRUSTING)) {
      camera_enabled = true;
      camera_start_time = osKernelGetTickCount();
    }

    if(camera_enabled){
    	HAL_GPIO_WritePin(CAMERA_GPIO_Port, CAMERA_Pin, GPIO_PIN_SET);
    	if(osKernelGetTickCount() > CAMERA_ON_TIME + camera_start_time){
    		camera_enabled = false;
    	}
    }
    else{
    	HAL_GPIO_WritePin(CAMERA_GPIO_Port, CAMERA_Pin, GPIO_PIN_RESET);
    }

    /* Enable Buzzer */
    if (buzzer_on_fsm ^ buzzer_on_telemetry) {
      if (buzzercounter > (800 / tick_update)) {

    	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
      }
      else{
      	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
      }
    }
    else{
    	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    }

    camera_state = camera_enabled;
    buzzer_state = buzzer_on_fsm ^ buzzer_on_telemetry;

    if (++buzzercounter >= 16){
    	buzzercounter = 0;
    }

    /* Sleep */
    osDelayUntil(tick_count);
  }
}

