/*
 * task_peripherals.c
 *
 *  Created on: Jun 15, 2020
 *      Author: Jonas
 */

#include "tasks/task_peripherals.h"
#include "main.h"

void user_pwm_setvalue(uint16_t value);

void vTaskPeripherals(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  osDelay(1200);
  HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_SET);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  user_pwm_setvalue(0);

  /* Camera Variables */
  uint32_t camera_counter = 0;
  bool camera_enabled = false;
  bool camera_wait = false;
  bool camera_trigger = false;
  bool camera_ready = false;

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

    /* Start Enable Camera Sequence */
    if ((telemetry_command == ENABLE_CAMERA) && !camera_enabled) {
      camera_enabled = true;
      HAL_GPIO_WritePin(CAMERA1_GPIO_Port, CAMERA1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(CAMERA2_GPIO_Port, CAMERA2_Pin, GPIO_PIN_SET);
      camera_counter = osKernelGetTickCount() + CAMERA_ON;
    }

    if ((camera_counter > osKernelGetTickCount()) && !camera_wait) {
      camera_wait = true;
      HAL_GPIO_WritePin(CAMERA1_GPIO_Port, CAMERA1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(CAMERA2_GPIO_Port, CAMERA2_Pin, GPIO_PIN_RESET);
      camera_counter = osKernelGetTickCount() + CAMERA_WAIT;
    }

    if ((camera_counter > osKernelGetTickCount()) && !camera_trigger) {
      camera_trigger = true;
      HAL_GPIO_WritePin(CAMERA1_GPIO_Port, CAMERA1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(CAMERA2_GPIO_Port, CAMERA2_Pin, GPIO_PIN_SET);
      camera_counter = osKernelGetTickCount() + CAMERA_TRIGGER;
    }

    if ((camera_counter > osKernelGetTickCount()) && !camera_ready) {
      camera_ready = true;
      HAL_GPIO_WritePin(CAMERA1_GPIO_Port, CAMERA1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(CAMERA2_GPIO_Port, CAMERA2_Pin, GPIO_PIN_RESET);
    }

    /* Camera first enable for some time, then turn off and finally turn on
     * again */

    /* Enable Buzzer */
    if (buzzer_on_fsm ^ buzzer_on_telemetry) {
      if (buzzercounter > (400 / tick_update)) {

    	user_pwm_setvalue(125);
        buzzercounter = 0;
      }
    }
    else{
    	user_pwm_setvalue(0);
    }

    buzzercounter++;

    /* Sleep */
    osDelayUntil(tick_count);
  }
}

void user_pwm_setvalue(uint16_t value)
{
    TIM_OC_InitTypeDef sConfigOC;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}
