#include "flash.h"
#include "Accelerometer.h"

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = NULL,
    .start_addr = ACCELEROMETER_DATA_ADRESS,
    .end_addr   = ACCELEROMETER_DATA_ADRESS+ACCELEROMETER_DATA_SIZE,
};

void fstorage_init(void) {
    ret_code_t rc;

    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}

void flash_write(uint32_t address, uint32_t *data, uint32_t length) {
    ret_code_t rc;

    rc = nrf_fstorage_write(&fstorage, address, data, length, NULL);
    APP_ERROR_CHECK(rc);

    while (nrf_fstorage_is_busy(&fstorage)) {
        __WFE();
    }
}

void flash_read(uint32_t address, uint32_t *data, uint32_t length) {
    memcpy(data, (uint32_t*)address, length * sizeof(uint32_t));
}