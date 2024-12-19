#ifndef Flash_H
#define Flash_H

#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "stdio.h"
#include "app_error.h"
#include <string.h>

void flash_read(uint32_t address, uint32_t *data, uint32_t length);
void flash_write(uint32_t address, uint32_t *data, uint32_t length);
void fstorage_init(void);

#endif