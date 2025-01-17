/**
 * Copyright (c) 2013 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**@file
 *
 * @defgroup ble_sdk_app_connectivity_main main.c
 * @{
 * @ingroup ble_sdk_app_connectivity
 *
 * @brief BLE Connectivity application.
 */

#include <stdbool.h>
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "nrf_sdh.h"
#include "ser_hal_transport.h"
#include "ser_conn_handlers.h"
#include "boards.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ser_phy_debug_comm.h"

#include "AudioFiles.h"
#include "Gpio.h"
#include "Flash.h"
#include "Timers.h"
#include "SleepMode.h"
#include "bluetooth.h"

extern bool write_accelerometer_data_command;
extern bool read_accel_data;


/***********************************************************************************************************************/


/***********************************************************************************************************************/

/**@brief Main function of the connectivity application. */
int main(void)
{
  //This is to ensure that the compiler stores the sound array in flash memory, as a declared but unused array will not be included in the code build----------
  size_t size;

  size=strlen(firsttrack);
  size=strlen(secondtrack);
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  
  uint32_t err_code = NRF_SUCCESS;

  // Initialize power management
  power_management_init();

  Gpio_init();

  Bluetooth_init();

  fstorage_init();

  //initial initialization of accelerometer data-------------------------------------------------------------
  flash_read((uint32_t)ACCELEROMETER_DATA_ADRESS, (uint32_t *)accelerometer_data , sizeof(accelerometer_data));

  if(accelerometer_data[0]==-1 && accelerometer_data[1]==-1 && accelerometer_data[2]==-1){
    memset(&accelerometer_data, 0, sizeof(accelerometer_data));
    Write_Data_Words(ACCELEROMETER_DATA_ADRESS, (uint32_t *)accelerometer_data, sizeof(accelerometer_data));
  }
  //---------------------------------------------------------------------------------------------------------

  BMA280Init();

    /* Enter main loop. */
    for (;;)
    {
        err_code = NRF_SUCCESS;

        if(read_accel_data){
          int16_t accel_buf[ACCEL_DATA_SIZE];

          bma280_read_accel_data(&accelerometer_data[0], &accelerometer_data[1], &accelerometer_data[2]);

          flash_read((uint32_t)ACCELEROMETER_DATA_ADRESS, (uint32_t *)accel_buf , sizeof(accel_buf));

          if(memcmp(accelerometer_data, accel_buf, ACCEL_DATA_SIZE)){
            Write_Data_Words(ACCELEROMETER_DATA_ADRESS, (uint32_t *)accelerometer_data, sizeof(accelerometer_data));
          }
          read_accel_data=false;
        }

        //if(write_accelerometer_data_command){
        //  Write_Data_Words(ACCELEROMETER_DATA_ADRESS, accelerometer_data, sizeof(accelerometer_data));
        //  write_accelerometer_data_command=false;
        //}

        sleep_mode_enter();

    }
}
/** @} */
