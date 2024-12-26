#include "SleepMode.h"

// Function to initialize power management
void power_management_init(void) {
    ret_code_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

// Function to enter sleep mode
void sleep_mode_enter(void) {
    // Prepare the system for sleep
    nrf_pwr_mgmt_run();
    
    //// Enter System ON sleep mode
    //__SEV();
    //__WFE();
    //__WFE();
}