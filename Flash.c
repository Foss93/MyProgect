#include "flash.h"
#include "Accelerometer.h"
#include "Bluetooth.h"

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = NULL,
    .start_addr = ACCELEROMETER_DATA_ADRESS,
    .end_addr   = ACCELEROMETER_DATA_ADRESS+0xFFF,
};

void fstorage_init(void)
{

    ret_code_t rc;

    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}

void write_data_to_flash(uint32_t address, uint32_t *data, uint32_t length)
{

    ret_code_t rc;

    int len = length%4;

    if(len){

      if(length<4){
        len=4;
      }
      else{
        len=length+(4-len);
      }

      uint32_t *newData = (uint32_t *)malloc(len); 

      memset(newData, 0, len);
      memcpy(newData, data, length);

      rc = nrf_fstorage_write(&fstorage, address, newData, len, NULL);
      APP_ERROR_CHECK(rc);
      free(newData);
    }
    else{
      rc = nrf_fstorage_write(&fstorage, address, data, length, NULL);
      APP_ERROR_CHECK(rc);
    }
}

void erase_flash_page(uint32_t address)
{

    ret_code_t rc;

    rc = nrf_fstorage_erase(&fstorage, address, 1, NULL);
    APP_ERROR_CHECK(rc);
}

void flash_read(uint32_t address, uint32_t *data, uint32_t length)
{
    memcpy(data, (uint32_t*)address, length);
}


void Write_Data_Words(uint32_t address, uint32_t *words, uint32_t num_words)
{

  erase_flash_page(address);
  write_data_to_flash(address, words, num_words);

}