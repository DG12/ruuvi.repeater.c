/**
 * Ruuvi Firmware 3.x advertisement tasks.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 **/

#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_communication_ble4_advertising.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_timer.h"
#include "ruuvi_interface_watchdog.h"
#include "task_advertisement.h"
#include "task_led.h"
#include "task_rtc.h"


#ifndef TASK_ADVERTISEMENT_LOG_LEVEL
  #define LOG_LEVEL RUUVI_INTERFACE_LOG_INFO
#else
  #define LOG_LEVEL TASK_ADVERTISEMENT_LOG_LEVEL
#endif
#define LOG(msg)  (ruuvi_interface_log(LOG_LEVEL, msg))

static ruuvi_interface_communication_t channel;

static void task_advertisement_repeat_task(void* p_event_data, uint16_t event_size)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_communication_ble4_scan_t* p_scan = (ruuvi_interface_communication_ble4_scan_t*) p_event_data;
  uint64_t address = 0;
  address |= (uint64_t)p_scan->addr[0] << 40;
  address |= (uint64_t)p_scan->addr[1] << 32;
  address |= p_scan->addr[2] << 24;
  address |= p_scan->addr[3] << 16;
  address |= p_scan->addr[4] << 8;
  address |= p_scan->addr[5] << 0;
  err_code |= ruuvi_interface_communication_radio_address_set(address);
  err_code |= ruuvi_interface_communication_ble4_advertising_send_raw(p_scan->data, p_scan->data_len);
}

static ruuvi_driver_status_t on_advertisement_event( const ruuvi_interface_communication_evt_t evt, void* p_data, size_t data_len)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  switch(evt)
  {
    case RUUVI_INTERFACE_COMMUNICATION_RECEIVED:
      LOG("Yay ");
      // TODO: Log data. 
      // Schedule repeat
      err_code |= ruuvi_interface_scheduler_event_put(p_data, data_len,
                                                      task_advertisement_repeat_task);
      ruuvi_interface_communication_ble4_advertising_scan_stop();
      task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_OFF);
      task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_ON);
      break;

    case RUUVI_INTERFACE_COMMUNICATION_SENT:
      LOG(":)\r\n");
      task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_ON);
      task_led_write(RUUVI_BOARD_LED_RED, TASK_LED_OFF);
      ruuvi_interface_communication_ble4_advertising_scan_start();
      break;

    default:
      LOG(":(");
      break;
  }
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  return RUUVI_DRIVER_SUCCESS;
}


ruuvi_driver_status_t task_advertisement_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  channel.on_evt = on_advertisement_event;
  err_code |= ruuvi_interface_communication_ble4_advertising_init(&channel);
  int8_t target_power = APPLICATION_ADVERTISING_POWER_DBM;
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_power_set(&target_power);
  err_code |= ruuvi_interface_communication_ble4_advertising_manufacturer_id_set(
                RUUVI_BOARD_BLE_MANUFACTURER_ID);
  err_code |= ruuvi_interface_communication_ble4_advertising_tx_interval_set(
                APPLICATION_ADVERTISING_INTERVAL_MS);
  return err_code;
}

ruuvi_driver_status_t task_advertisement_uninit(void)
{
  return ruuvi_interface_communication_ble4_advertising_uninit(&channel);
}

ruuvi_driver_status_t task_advertisement_scan_start(void)
{
  task_led_write(RUUVI_BOARD_LED_GREEN, TASK_LED_ON);
  return ruuvi_interface_communication_ble4_advertising_scan_start();
}

ruuvi_driver_status_t task_advertisement_start(void)
{
  return ruuvi_interface_communication_ble4_advertising_start();
}

ruuvi_driver_status_t task_advertisement_stop(void)
{
  // Ignore error code from stopping, as the advertisement will
  // always be stopped but INVALID_STATE is returned if
  // advertisement wasn't ongoing.
  ruuvi_interface_communication_ble4_advertising_stop();
  return RUUVI_DRIVER_SUCCESS;
}