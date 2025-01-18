#ifndef Timers_H
#define Temers_H

#include "app_error.h"
#include "app_timer.h"
#include "nRF5_interrupt.h"

void timer_init(void);
void timer_start(uint32_t timer_interval);
void timer_stop (void);

#endif