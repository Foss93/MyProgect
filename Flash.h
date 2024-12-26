#ifndef Flash_H
#define Flash_H

#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "stdio.h"
#include "app_error.h"
#include <string.h>
#include "nrf_sdh.h"
#include "nrf_nvmc.h"
#include <nrfx.h>

void flash_read(uint32_t address, uint32_t *data, uint32_t length);
//void flash_write(uint32_t address, uint32_t *data, uint32_t length);
void fstorage_init(void);

void Write_Data_Words(uint32_t address, uint32_t *words, uint32_t num_words);

#endif