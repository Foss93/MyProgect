#include "Timers.h"


#define TIMER_INTERVAL APP_TIMER_TICKS(5000) // 5000 ms (5 seconds)

APP_TIMER_DEF(m_timer_id); // Timer instance

void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

void timer_start(void)
{
    ret_code_t err_code = app_timer_start(m_timer_id, TIMER_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}