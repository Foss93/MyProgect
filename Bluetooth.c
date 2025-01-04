#include "bluetooth.h"
#include "AudioFiles.h"
#include "nRF5_interrupt.h"

ble_custom_service_t m_custom_service;

//NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                    /**< BLE GATT Queue instance. */
//               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
//               NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_GATT_DEF(m_gatt);                                                         // GATT module instance.
NRF_BLE_QWR_DEF(m_qwr);                                                           // Context for the Queued Write module.
BLE_ADVERTISING_DEF(m_advertising);                                               // Advertising module instance.                                                           /**< Context for the Queued Write module.*/


//BLE_HRS_C_DEF(m_hrs_c);
//BLE_RSCS_C_DEF(m_rscs_c);
//BLE_DB_DISCOVERY_DEF(m_db_discovery); // Определение экземпляра модуля обнаружения базы данных


void qwr_init(void)
{
    ret_code_t err_code;
    nrf_ble_qwr_init_t qwr_init;

    memset(&qwr_init, 0, sizeof(qwr_init));

    //qwr_init.mem_buffer.len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;
    qwr_init.error_handler = NULL;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);
}


void ble_stack_init(void)
{

    ret_code_t err_code;

    // Init SoftDevice
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configuration BLE stack
    uint32_t ram_start = 0;

        // Установка конфигурации соединения BLE
    //ble_cfg_t ble_cfg;
    //memset(&ble_cfg, 0, sizeof(ble_cfg));

    //ble_cfg.gap_cfg.role_count_cfg.periph_role_count = 1;
    //ble_cfg.gap_cfg.role_count_cfg.central_role_count = 1;
    //ble_cfg.gap_cfg.role_count_cfg.central_sec_count = 1;
    //err_code = sd_ble_cfg_set(BLE_GAP_CFG_CAR_INCL_CONFIG, &ble_cfg, ram_start);
    //APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Registering a BLE Event Handler
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, /*NULL*/ &m_custom_service);
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

    //err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    //APP_ERROR_CHECK(err_code);
}

void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    //ble_uuid_t m_adv_uuids[] = 
    //{
    //    {BLE_UUID_CUSTOM_SERVICE, BLE_UUID_TYPE_BLE}
    //};

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    //init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    //init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = 8000; // Advertising interval in 0.625 ms (5000 мс)
    init.config.ble_adv_fast_timeout  = 0;    // Advertising timeout in seconds (infinite timeout)

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
    cp_init.start_on_notify_cccd_handle    = BLE_CONN_HANDLE_INVALID;
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

    qwr_init();


    m_custom_service.conn_handle= BLE_CONN_HANDLE_INVALID;

    // Adding a custom UUID
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_custom_service.uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid.type = m_custom_service.uuid_type;
    ble_uuid.uuid = BLE_UUID_CUSTOM_SERVICE;

    // Adding service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY /*BLE_GATTS_SRVC_TYPE_INVALID*/, &ble_uuid, &m_custom_service.service_handle);
    APP_ERROR_CHECK(err_code);

    // Adding characteristic "sound"
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          char_uuid;
    uint8_t             initial_value[25];

    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0;
    char_md.p_cccd_md         = &cccd_md;

    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    //attr_md.rd_auth    = 0;
    //attr_md.wr_auth    = 0;
    //attr_md.vlen       = 0;

    char_uuid.type = m_custom_service.uuid_type;
    char_uuid.uuid = BLE_UUID_SOUND_CHAR;

    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(initial_value);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(initial_value);
    attr_char_value.p_value   = initial_value;


    err_code = sd_ble_gatts_characteristic_add(m_custom_service.service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_custom_service.sound_char_handles);
                                               
    APP_ERROR_CHECK(err_code);

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 0;
    char_md.p_cccd_md         = &cccd_md;


    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    //attr_md.wr_auth    = 0;
    //attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(accelerometer_read_data);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(accelerometer_read_data);
    attr_char_value.p_value   = (uint8_t*)accelerometer_read_data;
    


    // Adding characteristic "accelerometer"
    char_uuid.uuid = BLE_UUID_ACCEL_CHAR;

    err_code = sd_ble_gatts_characteristic_add(m_custom_service.service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_custom_service.accel_char_handles);
    APP_ERROR_CHECK(err_code);



}

void on_sound_char_write(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == ((ble_custom_service_t *)p_context)->sound_char_handles.value_handle) {
        uint8_t command = p_evt_write->data[0];

        switch (command) {
            case 0x01:
                play_melody(1);
                break;
            case 0x02:
                play_melody(2);
                break;
            case 0x03:
                stop_playing();
                break;
            default:
                // Ignore other command
                break;
        }
    }
}

void clear_ble_queue(uint16_t conn_handle, uint16_t char_handle)
{
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = char_handle;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len = 0;
    hvx_params.p_data = NULL;

    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
}

void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{

}

static void hrs_c_evt_handler(ble_hrs_c_t * p_hrs_c, ble_hrs_c_evt_t * p_hrs_c_evt)
{

}

//void db_discovery_init(void)
//{
//    ble_db_discovery_init_t db_init;

//    memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

//    db_init.evt_handler  = db_disc_handler;
//    db_init.p_gatt_queue = &m_ble_gatt_queue;

//    ret_code_t err_code = ble_db_discovery_init(&db_init);
//    APP_ERROR_CHECK(err_code);
//}

//void hrs_c_init(void)
//{
//    ret_code_t       err_code;
//    ble_hrs_c_init_t hrs_c_init_obj;

//    hrs_c_init_obj.evt_handler   = hrs_c_evt_handler;
//    hrs_c_init_obj.error_handler = NULL;
//    hrs_c_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

//    err_code = ble_hrs_c_init(&m_hrs_c, &hrs_c_init_obj);
//    APP_ERROR_CHECK(err_code);
//}

//static void rscs_c_init(void)
//{
//    ret_code_t        err_code;
//    ble_rscs_c_init_t rscs_c_init_obj;

//    rscs_c_init_obj.evt_handler   = NULL;
//    rscs_c_init_obj.error_handler = NULL;
//    rscs_c_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

//    err_code = ble_rscs_c_init(&m_rscs_c, &rscs_c_init_obj);
//    APP_ERROR_CHECK(err_code);
//}

//static bool dequeue_event(ble_event_t *event)
//{
//uint8_t queue_head = 0;

//    *event = event_queue[queue_head];
//    queue_head = (queue_head + 1) % QUEUE_SIZE;
//    return true;
//}

void Bluetooth_init (void){

  ret_code_t err_code;
  //qwr_init();
  ble_stack_init();
  gap_params_init();
  gatt_init();
  conn_params_init();
  custom_service_init();
  //db_discovery_init();
  advertising_init();
  
  //hrs_c_init();
  //rscs_c_init();
  //APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
  //app_sched_execute();// Start event Scheduler
  //NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, &m_custom_service);  
  err_code=ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);

   APP_ERROR_CHECK(err_code);

  //sd_ble_gap_adv_start(uint8_t adv_handle, uint8_t conn_cfg_tag)

}

