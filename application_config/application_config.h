/**
 * Application configuration constants
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef APPLICATION_CONFIG_H
#define APPLICATION_CONFIG_H
#include "application_modes.h" // Includes different modes, such as long-life with low sampling rate and tx rate.
#include "ruuvi_boards.h"      // Includes information such as number of buttons and leds onboard

/** @brief Version string, displayed in NFC read and GATT data on DIS */
#ifndef APPLICATION_FW_VERSION
  #if DEBUG
  #define APPLICATION_FW_VERSION "Ruuvi scanner 0.1.3-DEBUG"
  #else
  #define APPLICATION_FW_VERSION "Ruuvi scanner 0.1.3"
  #endif
#endif

/**
 * @brief enable compilation of NRF5 SDK15 implementation of driver interface functions.
 * Only one implementation can be enabled at a time.
 */
#define RUUVI_NRF5_SDK15_ENABLED              1

/**
 *  ADC configuration  -measures battery, synchronized to radio.
 */
#define APPLICATION_ADC_SAMPLERATE         RUUVI_DRIVER_SENSOR_CFG_MIN // Valid for continuous mode
#define APPLICATION_ADC_RESOLUTION         10
#define APPLICATION_ADC_SCALE              RUUVI_DRIVER_SENSOR_CFG_DEFAULT
#define APPLICATION_ADC_DSPFUNC            RUUVI_DRIVER_SENSOR_DSP_LAST
#define APPLICATION_ADC_DSPPARAM           1
#define APPLICATION_ADC_MODE               RUUVI_DRIVER_SENSOR_CFG_SINGLE // Note: call to task_adc_sample will leave the ADC in single-shot mode. Use data get only in continuous mode
#define APPLICATION_ADC_SAMPLE_INTERVAL_MS 3000 //!< configures minimum time before new ADC sample is taken

/**
 * Bluetooth configuration
 */
#ifndef APPLICATION_ADVERTISING_CONFIGURED
  #define APPLICATION_ADVERTISING_INTERVAL_MS           100 //!< Max allowed interval
  #define APPLICATION_ADVERTISING_POWER_DBM             RUUVI_BOARD_TX_POWER_MAX
  #define APPLICATION_SCAN_INTERVAL_MS                  3500
  #define APPLICATION_SCAN_WINDOW_MS                    APPLICATION_SCAN_INTERVAL_MS
  #define APPLICATION_SCAN_TIMEOUT_MS                   (3 * APPLICATION_SCAN_INTERVAL_MS)

#endif

/**
 * NFC configuration
 */
// Longest text in a text field, i.e. "FW: ojousima.logger.c 3.10.0"
#define APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE (32)
// Longest binary message
#define APPLICATION_COMMUNICATION_NFC_DATA_BUFFER_SIZE APPLICATION_COMMUNICATION_NFC_TEXT_BUFFER_SIZE
// 3 text records (version, address, id) and a data record
#define APPLICATION_COMMUNICATION_NFC_MAX_RECORDS      (4)
// 2 length bytes + 3 * text record + 1 * data record + 4 * 9 bytes for record header
// Conservers RAM for 3 * text buffer size + 1 * data buffer size + NDEF_FILE_SIZE
#define APPLICATION_COMMUNICATION_NFC_NDEF_FILE_SIZE   (166)

/**
 * Task scheduler configuration
 */
// Must fit entire advertisement + advertiser data. Use power of 2 in case future impl uses ruuvi ringbuffer.
#define APPLICATION_TASK_DATA_MAX_SIZE    64
// RAM conserved by scheduler is data size * queue length + overhead
#define APPLICATION_TASK_QUEUE_MAX_LENGTH 16

/**
 * Flags which determine which c-modules are compiled in.
 * These modules may reserve some RAM and FLASH, so if you
 * do not need module you can disable it. The modules might also
 * have some dependencies between themselves.
 */
#define APPLICATION_ACCELERATION_ENABLED              0
#define APPLICATION_ADC_ENABLED                       1
#define APPLICATION_ATOMIC_ENABLED                    1
#define APPLICATION_COMMUNICATION_ENABLED             1 // Common functions for communication
#define APPLICATION_COMMUNICATION_ADVERTISING_ENABLED 1 // BLE Advertising
#if NRF52811_XXAA
#define APPLICATION_COMMUNICATION_GATT_ENABLED        0 // BLE GATT, requires Flash and advertising
#else
#define APPLICATION_COMMUNICATION_GATT_ENABLED        0 // BLE GATT, requires Flash and advertising
#endif
#define APPLICATION_COMMUNICATION_NFC_ENABLED         RUUVI_BOARD_NFC_INTERNAL_INSTALLED
#if NRF52811_XXAA
#define APPLICATION_FLASH_ENABLED                     0
#else
#define APPLICATION_FLASH_ENABLED                     0
#endif
#define APPLICATION_GPIO_ENABLED                      1
#define APPLICATION_GPIO_INTERRUPT_ENABLED            0
#define APPLICATION_ENVIRONMENTAL_MCU_ENABLED         1
#define APPLICATION_ENVIRONMENTAL_BME280_ENABLED      0
#define APPLICATION_I2C_ENABLED                       0
#define APPLICATION_POWER_ENABLED                     1
#define APPLICATION_RTC_MCU_ENABLED                   (RUUVI_BOARD_RTC_INSTANCES > 2)
#define APPLICATION_SCHEDULER_ENABLED                 1
#define APPLICATION_SPI_ENABLED                       0
#define APPLICATION_TIMER_ENABLED                     1
#define APPLICATION_TIMER_MAX_INSTANCES               10 ///< Timers are allocated statically on RAM
#define APPLICATION_WATCHDOG_ENABLED                  1
#if DEBUG
  #define APPLICATION_WATCHDOG_INTERVAL_MS            1200000u
#else
  #define APPLICATION_WATCHDOG_INTERVAL_MS            30000u
#endif
#define APPLICATION_YIELD_ENABLED                     1

/** Number of GPIO interrupt lines */
#define APPLICATION_GPIO_INTERRUPT_NUMBER           (2 * APPLICATION_ACCELERATION_ENABLED \
                                                     + RUUVI_BOARD_BUTTONS_NUMBER)


#if DEBUG
#define APPLICATION_LOG_ENABLED                     1
#else 
#define APPLICATION_LOG_ENABLED                     0
#endif
// RUUVI_INTERFACE_LOG_ ERROR, WARNING, INFO, DEBUG
#define APPLICATION_LOG_LEVEL                       RUUVI_INTERFACE_LOG_INFO
/** @brief Bytes of RAM to conserve for printed log messages
 *  Pick a power of 2 for nRF5 backend. at least 128 is recommended.
 */
#define APPLICATION_LOG_BUFFER_SIZE                 256

// Choose one. RTT is recommended, but does not work on devices
// with readback protection enabled
#define APPLICATION_LOG_BACKEND_RTT_ENABLED         1
//#define APPLICATION_LOG_BACKEND_UART_ENABLED      0 // UART not implemented

#define  APPLICATION_FLASH_DATA_PAGES_NUMBER        RUUVI_BOARD_APP_PAGES

#endif