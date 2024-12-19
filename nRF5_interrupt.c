#include"nRF5_interrupt.h"

void timer_timeout_handler(void *p_context) {

}

void gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    if (pin == INTERRUPT_PIN) {

        //simulate that the data has changed from the accelerometer
        accelerometer_data[0]++;
        accelerometer_data[1]++;
        accelerometer_data[1]++;

        //write to memory
        flash_write(ACCELEROMETER_DATA_ADRESS, accelerometer_data, sizeof(accelerometer_data));

        //enter sleep mode
        sleep_mode_enter();
    }
}

void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
    ble_custom_service_t * p_custom_service = (ble_custom_service_t *)p_context;

    if (p_custom_service == NULL || p_ble_evt == NULL) {
        return;
    }

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            p_custom_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            p_custom_service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GATTS_EVT_WRITE:
              ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

              if (p_custom_service == NULL || p_ble_evt == NULL) {
                  return;
              }

              if (p_evt_write->handle == p_custom_service->sound_char_handles.value_handle) {
                on_sound_char_write(p_ble_evt, p_context);

              } else if (p_evt_write->handle == p_custom_service->accel_char_handles.value_handle) {
                  flash_read((uint32_t)ACCELEROMETER_DATA_ADRESS, accelerometer_read_data , sizeof(accelerometer_read_data));
              }
              
        case BLE_GAP_EVT_ADV_SET_TERMINATED://advertising end
            // Enter sleep mode
            sleep_mode_enter();

          break;

       default:
          break;
    }
}
