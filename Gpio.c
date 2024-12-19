#include "Gpio.h"

void gpiote_init(void) {
    ret_code_t err_code;

    if (!nrf_drv_gpiote_is_init()) {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
}

void gpio_interrupt_init(void) {
    ret_code_t err_code;

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(INTERRUPT_PIN, &in_config, gpio_interrupt_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(INTERRUPT_PIN, true);
}

void Gpio_init(void)
{
  gpiote_init();
  gpio_interrupt_init();
}