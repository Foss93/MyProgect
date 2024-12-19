#ifndef SleepMode_h
#define SleepMode_h

#include "nrf_pwr_mgmt.h"
#include "app_error.h"

void power_management_init(void);
void sleep_mode_enter(void);

#endif