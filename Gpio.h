#ifndef Gpio_H
#define Gpio_h

#include "nrf_drv_gpiote.h"
#include "nRF5_interrupt.h"

#define INTERRUPT_PIN  11  // Button1 (P0.11)

void Gpio_init(void);
void gpiote_init(void);

#endif