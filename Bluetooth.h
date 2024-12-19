#ifndef Bluetooth_H
#define Bluetooth_H

#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_conn_params.h"
#include "nRF5_interrupt.h"
#include "ble_srv_common.h"

#define DEVICE_NAME                     "MyDevice"
#define APP_BLE_CONN_CFG_TAG            1                                         /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                         /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)          /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                         /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)           /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                     /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                    /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                         /**< Number of attempts before giving up the connection parameter negotiation. */

NRF_BLE_GATT_DEF(m_gatt);                                                         /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                           /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                               /**< Advertising module instance. */

#define BLE_UUID_CUSTOM_SERVICE_BASE {0x23, 0x15, 0x12, 0xE1, 0x5A, 0xA5, 0xE4, 0x9E, 0xA1, 0x4A, 0xB1, 0xC8, 0x00, 0x00, 0x00, 0x00}
#define BLE_UUID_CUSTOM_SERVICE 0x1400
#define BLE_UUID_SOUND_CHAR     0x1401
#define BLE_UUID_ACCEL_CHAR     0x1402

typedef struct
{
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    sound_char_handles;
    ble_gatts_char_handles_t    accel_char_handles;
    uint8_t                     uuid_type;
    uint16_t conn_handle;
} ble_custom_service_t;

extern ble_custom_service_t m_custom_service;

void on_sound_char_write(ble_evt_t const * p_ble_evt, void * p_context);
void Bluetooth_init(void);

#endif