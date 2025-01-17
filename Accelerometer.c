//This file will contain the library for working with and initializing the external accelerometer
#include "Accelerometer.h"
#include "Gpio.h"
#include "nRF5_interrupt.h"

int16_t accelerometer_data[ACCEL_DATA_SIZE];
uint8_t dt;

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
//bool twim_xfer_done = false;
extern bool twim_xfer_done;

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
}

uint8_t bma280_read_register(uint8_t reg)
{
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

    return rx_buf;
}

void bma280_gpio_interrupt_init(void)
{
    ret_code_t err_code;

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(BMA_INTERRUPT_PIN, &in_config, bma280_gpio_interrupt_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(BMA_INTERRUPT_PIN, true);
}

void bma280_read_accel_data(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[6];
    uint8_t reg = 0x02; // Начальный регистр данных акселерометра

    // Чтение 6 байт данных акселерометра
    twim_xfer_done = false;
    ret_code_t err_code = nrf_drv_twi_tx(&m_twi, BMA280_ADDRESS, &reg, 1, false);
    APP_ERROR_CHECK(err_code);

    while (!twim_xfer_done) {
        __WFE();
    }

    twim_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, BMA280_ADDRESS, data, 6);
    APP_ERROR_CHECK(err_code);

    while (!twim_xfer_done) {
        __WFE();
    }

    // Обработка данных акселерометра
    *x = (int16_t)((data[1] << 8) | data[0]) >> 4;
    *y = (int16_t)((data[3] << 8) | data[2]) >> 4;
    *z = (int16_t)((data[5] << 8) | data[4]) >> 4;
}

void BMA280Init(void)
{
    twi_init();
    bma280_gpio_interrupt_init();

    bma280_write_register(0x14, 0xB6);

    nrf_delay_ms(2000);

    bma280_write_register(0x0F, 0x03);  // Установить диапазон ±2g
    bma280_write_register(0x10, 0x08);  // Установить полосу пропускания 7.81Hz
    bma280_write_register(0x20, 0x0A);  // INT1 конфигурация: faling edge
    bma280_write_register(0x21, 0x00);  
    bma280_write_register(0x16, 0x40);  // Enable interrupt 0
    bma280_write_register(0x19, 0x40);  // Interrupt map
     bma280_write_register(0x1B, 0x40);  // Interrupt map
    //bma280_write_register(0x1A, 0x01);  // map FIFO full interrupt to INT1

    bma280_write_register(0x11, 0x00);  // Normal mode

    nrf_delay_ms(2000);
}
