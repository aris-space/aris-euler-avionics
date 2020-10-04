/*
 * task_gps.c
 *
 *  Created on: Jun 26, 2020
 *      Author: Luca Jost
 */

#include "util/logging_util.h"
#include "tasks/task_battery.h"

static float get_temp(uint16_t adc_value);

uint32_t adc_value[4];

void vTaskBattery(void *argument) {
  /* For periodic update */
  uint32_t tick_count, tick_update;

  /* Initialise Variables */
  double mah;
  double curr = 0;
  double supp = 0;
  double bat = 0;

  battery_data_t battery_data = {0};

  int counter = 0;

  /* Infinite loop */
  tick_count = osKernelGetTickCount();
  tick_update = osKernelGetTickFreq() / BATTERY_SAMPLE_RATE;

  // ADC init

  HAL_ADC_Stop_DMA(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, adc_value, 4);

  osDelay(500);

  for (;;) {
    tick_count += tick_update;
    double current2 = ((double)adc_value[0] * (2.5 / 65536.0) - (3.3 * 0.107)) /
                      0.264;                                       // CURR2
    float supply_voltage = (adc_value[2] / 65536.0) * 5;           // 3V3
    float battery_voltage = adc_value[3] * (2.5 / 65536.0) * 5.2;  // BAT
    double current1 = ((double)adc_value[1] * (2.5 / 65536.0) - (3.3 * 0.107)) /
                      0.264;  // CURR1

    if ((adc_value[0] | adc_value[1]) == 0) {
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_ADC_Start_DMA(&hadc1, adc_value, 4);
    }
    // Filter adc values
    if (counter < 50) {
      counter++;
      curr += current1 + current2;
      supp += supply_voltage;
      bat += battery_voltage;
    } else {
      counter = 0;
      mah += (curr) / (BATTERY_SAMPLE_RATE * 3.6);
      battery_data.consumption = (uint16_t)mah;
      battery_data.current = (uint16_t)(curr * 1000) / 50;
      battery_data.power = (curr * 1000) / 50 * (battery_voltage / 50);
      battery_data.supply = (uint16_t)(supp * 20);
      battery_data.battery = (uint16_t)(bat * 20);
      curr = 0;
      bat = 0;
      supp = 0;
      usb_print(
          "[BATTERY] Supply: %d Battery: %d Current: %dmA, Consumption: %dmAh "
          "Power: %dmW\n",
          battery_data.supply, battery_data.battery, battery_data.current,
          battery_data.consumption, battery_data.power);

      /* Log Battery Power */
      log_sensor(osKernelGetTickCount(), 4, BATTERY, &battery_data);

      /* Write Data into global Variable */
      if (acquire_mutex(&battery_mutex) == osOK) {
        global_battery_data.battery = battery_data.battery;
        global_battery_data.consumption = battery_data.consumption;
        global_battery_data.current = battery_data.current;
        release_mutex(&battery_mutex);
      }
    }
    /* Sleep */
    osDelayUntil(tick_count);
  }
}

static float get_temp(uint16_t adc_value) {
  float VSENSE;
  VSENSE = 2.5 / 4096 * adc_value;
  return ((V25 - VSENSE) / AVG_SLOPE + 25);
}
