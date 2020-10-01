/*
 * task_motor_control.c
 *
 *  Created on: May 23, 2020
 *      Author: Jonas
 */

#include "util/logging_util.h"
#include "tasks/task_motor_control.h"
#include "drivers/epos4/epos4.h"

static void testairbrakes(int32_t position);

void vTaskMotorCont(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  osStatus_t motor_status = osOK;

  /* Telemetry Command and FSM State */
  command_e telemetry_command = IDLE_COMMAND;
  flight_phase_detection_t flight_phase_detection = {0};
  flight_phase_detection.flight_phase = IDLE;
  flight_phase_detection.mach_number = SUBSONIC;

  /* Initialisation */
   int8_t position_mode = 0x08;
  /* Profile Position Mode */
  //int8_t position_mode = 0x01;
  int32_t PPM_velocity = 10000;
  int32_t PPM_acceleration = 100000;
  int32_t PPM_deceleration = 100000;

  osDelay(3000);

  /* Controller Variables */
  int32_t controller_actuation = 0;

  /* Task Variables */
  int32_t desired_motor_position = 0;
  int32_t measured_motor_position = 0;

  while (EnableMotor() != osOK) {
    osDelay(1000);
  };

  SetPositionMode(position_mode);
  while (SetPositionMode(position_mode) != osOK) {
    osDelay(1000);
  };

  if (position_mode == 0x01) {
    motor_status =
        ConfigurePPM(PPM_velocity, PPM_acceleration, PPM_deceleration);
  }

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / MOTOR_TASK_FREQUENCY;

  for (;;) {
    tick_count += tick_update;

    /* Read Current Motor Position */
    motor_status = GetPosition(&measured_motor_position);

    /* Transform Motor Position to a value between [0-1000] */
    measured_motor_position = measured_motor_position * 1000 / (-150);

    /* Write To global airbrake extension */
    if (AcquireMutex(&airbrake_ext_mutex) == osOK) {
      global_airbrake_ext_meas = measured_motor_position;
      ReleaseMutex(&airbrake_ext_mutex);
    }

    /* Read Telemetry Command */
    ReadMutex(&command_mutex, &global_telemetry_command, &telemetry_command,
              sizeof(global_telemetry_command));

    UsbPrint("[MOTOR] Read Position:%d\n", measured_motor_position);

    /* Read FSM State */
    ReadMutex(&fsm_mutex, &global_flight_phase_detection,
              &flight_phase_detection, sizeof(global_flight_phase_detection));

    /* Read in Current Controller Output */
    ReadMutex(&controller_mutex, &controller_output_global,
              &controller_actuation, sizeof(controller_actuation));

    /* Transform 0-1 Controller output to controller output of motor */
    desired_motor_position =
        (int32_t)(((float)controller_actuation) / 1000 * (-150));

    /* Check Bounds */
    if (desired_motor_position > 2) {
      desired_motor_position = 2;
    }

    if (desired_motor_position < -150) {
      desired_motor_position = -150;
    }

    /* If we are in IDLE, THRUSTING or DESCENDING
     * the Motor is not allowed to Move!
     */
    if (flight_phase_detection.flight_phase == COASTING) {
      /* Move the Motor */
      motor_status = MoveToPositionPPM(desired_motor_position);
    } else {
      motor_status = MoveToPositionPPM(50);
    }

    if(osKernelGetTickCount() < 10000 && osKernelGetTickCount() > 9960){
    	testairbrakes(-100);
    }

    /* Airbrake Test if telemetry command is given and we are in idle state */
    if (flight_phase_detection.flight_phase == IDLE &&
        telemetry_command == AIRBRAKE_TEST_COMMAND &&
        osKernelGetTickCount() < 60000) {
      testairbrakes(-140);
      telemetry_command = IDLE_COMMAND;
    }

    /* Log Motor Position and Desired Motor Position */
    logMotor(osKernelGetTickCount(), desired_motor_position,
             measured_motor_position);

    if (motor_status != osOK && flight_phase_detection.flight_phase == IDLE) {
      DisableMotor();
      osDelay(1000);
      EnableMotor();
    }

    osDelayUntil(tick_count);
  }
}

static void testairbrakes(int32_t position) {
  MoveToPositionPPM(position);
  osDelay(100);
  MoveToPositionPPM(2);
}
