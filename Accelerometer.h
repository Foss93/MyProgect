#ifndef Accelerometer_H
#define Accelerometer_H

#include "stdint.h"

#define ACCELEROMETER_DATA_ADRESS 0x000FFFC0 //уточнить обязательно
#define ACCELEROMETER_DATA_SIZE 0x40

extern uint32_t accelerometer_data[3];
extern uint32_t accelerometer_read_data[3];


#endif