#include "task_gpio.h"
#include "ruuvi_boards.h"
#include "ruuvi_interface_gpio.h"
#include <stddef.h>

ruuvi_driver_status_t task_gpio_init()
{
  ruuvi_driver_status_t err_code = RUUVI_DRIVER_SUCCESS;
  err_code |= ruuvi_interface_gpio_init();
  // Setup SPI Chip selects to high

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  ruuvi_interface_gpio_id_t cs[] = RUUVI_BOARD_SPI_SS_LIST;
#pragma GCC diagnostic pop

  for(size_t ii = 0; ii < sizeof(cs); ii++)
  {
    ruuvi_interface_gpio_configure(cs[ii], RUUVI_INTERFACE_GPIO_MODE_OUTPUT_STANDARD);
    ruuvi_interface_gpio_write(cs[ii], RUUVI_INTERFACE_GPIO_HIGH);
  }

  return err_code;
}