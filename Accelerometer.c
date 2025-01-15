//This file will contain the library for working with and initializing the external accelerometer
#include "Accelerometer.h"
#include "Gpio.h"

uint32_t accelerometer_data[3]={0,0,0};
uint8_t dt;

//uint32_t accelerometer_read_data[3];

//static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  // SPI instance
//static volatile bool spi_xfer_done;
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
volatile bool twim_xfer_done = false;

//void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
//{
//  spi_xfer_done=true;
//}

//void BMA280SpiInit(void)
//{
//    ret_code_t           err_code;
//    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

//    spi_config.ss_pin   = SPI_SS_PIN;
//    spi_config.miso_pin = SPI_MISO_PIN;
//    spi_config.mosi_pin = SPI_MOSI_PIN;
//    spi_config.sck_pin  = SPI_SCK_PIN;
//    spi_config.mode     = NRF_DRV_SPI_MODE_2;
//    //spi_config.orc      = 0x00;
//    spi_config.frequency = NRF_DRV_SPI_FREQ_4M;
//    //spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;
//    err_code=nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);
//    APP_ERROR_CHECK(err_code);

//    //gpiote_init();

//    //nrf_gpio_cfg_output(SPI_SS_PIN);
//    ////nrf_gpio_pin_set(SPI_SS_PIN);
//    //nrf_gpio_pin_clear(SPI_SS_PIN);
//}

void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context) {
        if (p_event->type == NRFX_TWIM_EVT_DONE) {
        twim_xfer_done = true;
    }
}

void twi_init(void) {
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = SCL_PIN,
       .sda                = SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

void bma280_write_register(uint8_t reg, uint8_t value)
{
    uint8_t tx_buf[2] = {reg, value};
    uint8_t rx_buf;
    uint32_t error_code;
twim_xfer_done = false;
    error_code = nrf_drv_twi_tx(&m_twi, BMA280_ADDRESS, tx_buf, 2, false);
    APP_ERROR_CHECK(error_code);

        while (!twim_xfer_done) {
        __WFE();
    }

    //spi_xfer_done=false;
    ////nrf_gpio_pin_clear(SPI_SS_PIN);
    //error_code=nrf_drv_spi_transfer(&spi, tx_buf, 2, NULL, 0);

    //while(!spi_xfer_done){}
    //    //nrf_gpio_pin_set(SPI_SS_PIN);
}

uint8_t bma280_read_register(uint8_t reg)
{
    //uint8_t tx_buf[2] = {reg | 0x80, 0x00};  // Установка бита чтения
    //uint8_t rx_buf[4];
    //uint8_t tx_buf = reg | 0x80;  // Установка бита чтения (для SPI)
    uint8_t rx_buf;
    uint32_t error_code;
twim_xfer_done = false;
    error_code = nrf_drv_twi_tx(&m_twi, BMA280_ADDRESS, &reg, 1, false);
    APP_ERROR_CHECK(error_code);

        while (!twim_xfer_done) {
        __WFE();
    }
twim_xfer_done = false;
    error_code = nrf_drv_twi_rx(&m_twi, BMA280_ADDRESS, &rx_buf, 1);
    APP_ERROR_CHECK(error_code);

        while (!twim_xfer_done) {
        __WFE();
    }


    //spi_xfer_done=false;
    ////nrf_gpio_pin_clear(SPI_SS_PIN);
    //error_code=nrf_drv_spi_transfer(&spi, &tx_buf, 1, &rx_buf, 1);

    //while(!spi_xfer_done){}
    //    //nrf_gpio_pin_set(SPI_SS_PIN);
    ////return rx_buf[1];
    return rx_buf;
}

void BMA280Init(void)
{
    //BMA280SpiInit();
    //nrf_delay_ms(2000);

    twi_init();

    //    uint8_t tx_buf[] = {0x11};  // Команда для чтения идентификатора устройства
    //uint8_t rx_buf[1];
    //nrf_drv_spi_transfer(&spi, tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf));
    //bma280_write_register(0x14, 0xB6);
    //nrf_delay_ms(2000);
    bma280_write_register(0x0F, 0x05);
    dt=bma280_read_register(0x0F);
}
