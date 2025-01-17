#include"nRF5_interrupt.h"

bool write_accelerometer_data_command=false;
bool twim_xfer_done = false;
bool read_accel_data = false;

void timer_timeout_handler(void *p_context)
{

}

void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context) {
        if (p_event->type == NRFX_TWIM_EVT_DONE) {
        twim_xfer_done = true;
    }
}

void bma280_gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  if (pin == BMA_INTERRUPT_PIN) {

    read_accel_data=true;

  }
}

void gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) //simulate that we have event from accelerometer
{
    if (pin == INTERRUPT_PIN) {

      read_accel_data=true;

      //read data from accelerometer

      //simulate that the data has changed from the accelerometer
      //flash_read((uint32_t)ACCELEROMETER_DATA_ADRESS, (uint32_t*)accelerometer_data , sizeof(accelerometer_data));

      //accelerometer_data[0]++;
      //accelerometer_data[1]++;
      //accelerometer_data[2]++;

      ////write to memory
      //write_accelerometer_data_command=true;

    }
}

void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_custom_service_t * p_custom_service = (ble_custom_service_t *)p_context;    
    
    uint32_t error_code;

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

            if (p_evt_write->handle == p_custom_service->sound_char_handles.value_handle) {
              on_sound_char_write(p_ble_evt, p_context);
            }
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
              ble_gatts_evt_rw_authorize_request_t const * p_auth_req = &p_ble_evt->evt.gatts_evt.params.authorize_request;              

              if(p_auth_req == NULL){
                return;
              }
              if (p_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_READ) {
                  ble_gatts_rw_authorize_reply_params_t auth_reply;
 
                  memset(&auth_reply, 0, sizeof(auth_reply));
                  auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;

                  if (p_auth_req->request.read.handle == p_custom_service->accel_char_handles.value_handle) {

                    flash_read((uint32_t)ACCELEROMETER_DATA_ADRESS, accelerometer_data , sizeof(accelerometer_data));

                    auth_reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
                    auth_reply.params.read.update = 1;
                    auth_reply.params.read.offset = 0;
                    auth_reply.params.read.len = sizeof(accelerometer_data);
                    auth_reply.params.read.p_data = (uint8_t*)accelerometer_data;

                  } else {
                      auth_reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND;
                  }

                  error_code = sd_ble_gatts_rw_authorize_reply(p_custom_service->conn_handle, &auth_reply);

              }

          break;
              
        case BLE_GAP_EVT_ADV_SET_TERMINATED://advertising end

          break;

       default:
          break;
    }
}
