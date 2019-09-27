#include "application_config.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"
#include "ruuvi_interface_adc.h"
#include "ruuvi_interface_adc_mcu.h"
#include "ruuvi_interface_communication_radio.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_scheduler.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_timer.h"
#include "task_adc.h"
#include "task_led.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>

static ruuvi_driver_sensor_t adc_sensor = {0};
static volatile uint64_t t_sample = 0;
static volatile float after_tx = 0;
ruuvi_driver_status_t task_adc_configure(ruuvi_driver_sensor_configuration_t* config)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO,
                      "\r\nAttempting to configure ADC with:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "V");
  err_code |= adc_sensor.configuration_set(&adc_sensor, config);
  RUUVI_DRIVER_ERROR_CHECK(err_code, ~RUUVI_DRIVER_ERROR_FATAL);
  ruuvi_interface_log(RUUVI_INTERFACE_LOG_INFO, "Actual configuration:\r\n");
  ruuvi_interface_log_sensor_configuration(RUUVI_INTERFACE_LOG_INFO, config, "V");
  return err_code;
}

ruuvi_driver_status_t task_adc_init(void)
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  ruuvi_driver_bus_t bus = RUUVI_DRIVER_BUS_NONE;
  uint8_t handle = RUUVI_INTERFACE_ADC_AINVDD;
  ruuvi_driver_sensor_configuration_t config;
  config.samplerate    = APPLICATION_ADC_SAMPLERATE;
  config.resolution    = APPLICATION_ADC_RESOLUTION;
  config.scale         = APPLICATION_ADC_SCALE;
  config.dsp_function  = APPLICATION_ADC_DSPFUNC;
  config.dsp_parameter = APPLICATION_ADC_DSPPARAM;
  config.mode          = APPLICATION_ADC_MODE;
  err_code |= ruuvi_interface_adc_mcu_init(&adc_sensor, bus, handle);
  RUUVI_DRIVER_ERROR_CHECK(err_code, RUUVI_DRIVER_SUCCESS);

  if(RUUVI_DRIVER_SUCCESS == err_code)
  {
    err_code |=  task_adc_configure(&config);
    return err_code;
  }
  // Return error if ADC could not be configured
  return RUUVI_DRIVER_ERROR_NOT_FOUND;
}

ruuvi_driver_status_t task_adc_sample(void)
{
  if(NULL == adc_sensor.mode_set) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  uint8_t mode = RUUVI_DRIVER_SENSOR_CFG_SINGLE;
  return adc_sensor.mode_set(&mode);
}

ruuvi_driver_status_t task_adc_data_get(ruuvi_interface_adc_data_t* const data)
{
  if(NULL == data) { return RUUVI_DRIVER_ERROR_NULL; }

  if(NULL == adc_sensor.data_get) { return RUUVI_DRIVER_ERROR_INVALID_STATE; }

  return adc_sensor.data_get(data);
}