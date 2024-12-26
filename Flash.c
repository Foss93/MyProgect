#include "flash.h"
#include "Accelerometer.h"
#include "Bluetooth.h"

//<ProgramSection alignment="4" keep="Yes" load="Yes" name=".fs_data" start="0x000FF000" size="0x1000" address_symbol="__start_fs_data" end_symbol="__stop_fs_data" />

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = NULL,
    .start_addr = ACCELEROMETER_DATA_ADRESS,
    .end_addr   = ACCELEROMETER_DATA_ADRESS+0xFFF,
};

// Функция инициализации fstorage
void fstorage_init(void)
{
    ret_code_t rc;

    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    //    if (p_fs_api == NULL)
    //{
    //    //NRF_LOG_ERROR("p_fs_api is NULL");
    //    return;
    //}

    //    // Дополнительная проверка полей структуры p_fs_api
    //if (p_fs_api->init == NULL || p_fs_api->uninit == NULL || p_fs_api->read == NULL || p_fs_api->write == NULL || p_fs_api->erase == NULL)
    //{
    //    //NRF_LOG_ERROR("One or more function pointers in p_fs_api are NULL");
    //    return;
    //}

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
}

// Функция записи данных во флэш-память
void write_data_to_flash(uint32_t address, uint32_t *data, uint32_t length)
{
    ret_code_t rc;

    rc = nrf_fstorage_write(&fstorage, address, data, length, NULL);
    APP_ERROR_CHECK(rc);
}

// Функция стирания страницы флэш-памяти
void erase_flash_page(uint32_t address)
{
    ret_code_t rc;
    if (address % 4 != 0)
    {
        //NRF_LOG_ERROR("Address is not aligned: 0x%08x", address);
        return;
    }

    rc = nrf_fstorage_erase(&fstorage, address, 1, NULL);
    APP_ERROR_CHECK(rc);
}

void flash_read(uint32_t address, uint32_t *data, uint32_t length) {
    memcpy(data, (uint32_t*)address, length);
}


void Write_Data_Words(uint32_t address, uint32_t *words, uint32_t num_words)
{

  erase_flash_page(address);
  write_data_to_flash(address, words, num_words);
  //uint8_t critical_section_state;
  //uint32_t err_code;

  //err_code = nrf_sdh_disable_request();
  //APP_ERROR_CHECK(err_code);

  //nrf_nvmc_page_erase(address);
  //nrf_nvmc_write_words(address, words, num_words);

  //err_code = nrf_sdh_enable_request();
  //APP_ERROR_CHECK(err_code);


}