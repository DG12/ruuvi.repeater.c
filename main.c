/**
 * Ruuvi Firmware 3.x code. Reads the sensors onboard RuuviTag and broadcasts the sensor data in a manufacturer specific format.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_boards.h"
#include "task_adc.h"
#include "task_advertisement.h"
#include "task_gpio.h"
#include "task_led.h"
#include "task_nfc.h"
#include "task_power.h"
#include "task_rtc.h"
#include "task_scheduler.h"
#include "task_timer.h"
#include "test_adc.h"
#include <stdio.h>

/** Run tests which rely only on MCU. 
 *  These tests require relevant peripherals being uninitialized
 *  before tests and leave the peripherals uninitialized.
 *  Production firmware should not run these tests. 
 */
static void run_mcu_tests()
{
  #if RUUVI_RUN_TESTS
  // Use task_rtc function to apply offset configured by user to sensor values.
  ruuvi_driver_sensor_timestamp_function_set(ruuvi_interface_rtc_millis);
  ruuvi_interface_rtc_init();
  test_adc_run();
  ruuvi_interface_delay_ms(1000);
  ruuvi_interface_rtc_uninit();
  #endif
}

/*
 * Initialize MCU peripherals. 
 */
static void init_mcu(void)
{
  // Init watchdog
  ruuvi_interface_watchdog_init(APPLICATION_WATCHDOG_INTERVAL_MS);

  // Init yield
  ruuvi_driver_status_t status = ruuvi_interface_yield_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  // Init GPIO
  status = task_gpio_init();
  // Initialize LED gpio pins
  status |= task_led_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  // Initialize RTC, timer and scheduler. Enable low-power sleep.
  // Dummy implementation of RTC requires timer.
  status = task_timer_init();
  status |= task_rtc_init();
  status |= task_scheduler_init();
  status |= ruuvi_interface_yield_low_power_enable(true);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  // Initialize power
  status |= task_power_dcdc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  // Initialize ADC
  status |= task_adc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
}

static void init_comms(void)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;

  #if APPLICATION_COMMUNICATION_NFC_ENABLED
  // Initialize nfc
  status |= task_nfc_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  #endif

  #if APPLICATION_COMMUNICATION_ADVERTISING_ENABLED
  // Initialize BLE - and start advertising.
  status = task_advertisement_init();
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  #endif
}

// Init logging
static void init_logging(void)
{
  ruuvi_driver_status_t status = RUUVI_DRIVER_SUCCESS;
  status |= ruuvi_interface_log_init(APPLICATION_LOG_LEVEL);
  RUUVI_DRIVER_ERROR_CHECK(status, RUUVI_DRIVER_SUCCESS);
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Program start \r\n");
}

int main(void)
{
  init_logging();   // Initializes logging to user console
  run_mcu_tests();  // Runs tests which do not rely on MCU peripherals being initialized
  init_mcu();       // Initialize MCU peripherals, except for communication with users. 

  task_led_write(RUUVI_BOARD_LED_ACTIVITY, TASK_LED_ON); // Turn activity led on
   // Initialize communication with outside world - BLE, NFC, Buttons
   /* IMPORTANT! After this point pausing the program flow asserts,
    * since softdevice asserts on missed timer. This includes debugger.
    */
  init_comms();    

  // Turn activity led off. Turn status_ok led on if no errors occured
  task_led_write(RUUVI_BOARD_LED_ACTIVITY, TASK_LED_OFF);
  task_led_activity_led_set(RUUVI_BOARD_LED_ACTIVITY);
  if(RUUVI_DRIVER_SUCCESS == ruuvi_driver_errors_clear())
  {
    task_led_write(RUUVI_BOARD_LED_STATUS_OK, TASK_LED_ON);
    task_led_activity_led_set(RUUVI_BOARD_LED_STATUS_OK);
    ruuvi_interface_delay_ms(1000);
  }

  // Turn LEDs off
  task_led_write(RUUVI_BOARD_LED_STATUS_OK, TASK_LED_OFF);
  // Configure activity indication - use advertisement state instead
  // ruuvi_interface_yield_indication_set(task_led_activity_indicate);
  // Start scanning. On timeout queue is sent.
  task_advertisement_scan_start();
  while(1)
  {
    // Execute scheduled tasks
    ruuvi_interface_scheduler_execute();
    // Sleep until something happens
    ruuvi_interface_yield();
  }
}