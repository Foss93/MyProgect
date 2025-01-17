#ifndef nRF5_interrupt_H
#define nRF5_interrupt_H

#include "Flash.h"
#include "Gpio.h"
#include "Accelerometer.h"
#include "Bluetooth.h"
#include "SleepMode.h"

void gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void timer_timeout_handler(void *p_context);
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
void bma280_gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);



#endif
