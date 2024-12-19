#include "bluetooth.h"

ble_custom_service_t m_custom_service;


void ble_stack_init(void)
{
    ret_code_t err_code;

    // Init SoftDevice
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configuration BLE stack
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Registering a BLE Event Handler
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = 8000; // Интервал рекламирования в 0.625 мс единицах (5000 мс)
    init.config.ble_adv_fast_timeout  = 0;    // Таймаут рекламирования в секундах (0 означает бесконечный таймаут)

    init.evt_handler = NULL;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = NULL;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

void custom_service_init(void)
{
    ret_code_t          err_code;
    ble_uuid_t          ble_uuid;
    ble_uuid128_t       base_uuid = {BLE_UUID_CUSTOM_SERVICE_BASE};

    // Добавление пользовательского UUID
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_custom_service.uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid.type = m_custom_service.uuid_type;
    ble_uuid.uuid = BLE_UUID_CUSTOM_SERVICE;

    // Добавление сервиса
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &m_custom_service.service_handle);
    APP_ERROR_CHECK(err_code);

    // Добавление характеристики "звук"
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          char_uuid;
    uint8_t             initial_value = 0;

    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1;
    char_md.p_cccd_md         = &cccd_md;

    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    char_uuid.type = m_custom_service.uuid_type;
    char_uuid.uuid = BLE_UUID_SOUND_CHAR;

    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);
    attr_char_value.p_value   = &initial_value;

    err_code = sd_ble_gatts_characteristic_add(m_custom_service.service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_custom_service.sound_char_handles);
    APP_ERROR_CHECK(err_code);

    // Добавление характеристики "акселерометр"
    char_uuid.uuid = BLE_UUID_ACCEL_CHAR;

    err_code = sd_ble_gatts_characteristic_add(m_custom_service.service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_custom_service.accel_char_handles);
    APP_ERROR_CHECK(err_code);
}

void on_sound_char_write(ble_evt_t const * p_ble_evt, void * p_context) {
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == ((ble_custom_service_t *)p_context)->sound_char_handles.value_handle) {
        uint8_t command = p_evt_write->data[0];

        switch (command) {
            case 0x01:
                // play_first_melody();
                break;
            case 0x02:
                //play_second_melody();
                break;
            case 0x03:
                // stop_playing();
                break;
            default:
                // Ignore other command
                break;
        }
    }
}

void Bluetooth_init (void)
{
  ble_stack_init();
  gap_params_init();
  gatt_init();
  advertising_init();
  conn_params_init();
  custom_service_init();
}

