#ifndef Accelerometer_H
#define Accelerometer_H

#include "stdint.h"
#include "nrf_delay.h"

#define ACCELEROMETER_DATA_ADRESS 0x000FF000
#define ACCELEROMETER_DATA_SIZE 0xC

extern uint32_t accelerometer_data[3];
//extern uint32_t accelerometer_read_data[3];


#endif