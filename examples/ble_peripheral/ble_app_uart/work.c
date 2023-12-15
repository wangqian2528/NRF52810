#include "work.h"
#include "pressure.h"
#include "pid.h"

#include "nrf_drv_pwm.h"
#include "nrf_gpio.h"

#include "nrf_log.h"

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_common_t m_seq_values;
static nrf_pwm_sequence_t const m_seq = {
    .values.p_common = &m_seq_values,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

work_state_t g_work_state = WORK_START;

static void valve_ctl(uint8_t set)
{
    if (set == 0)
    {
        // nrfx_pwm_stop(&m_pwm0, false);
        nrf_gpio_pin_write(12, 0);
    }
    else
    {
        // m_seq_values = 60;
        // nrf_drv_pwm_simple_playback(&m_pwm0, &m_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
        nrf_gpio_pin_write(12, 1);
    }
}

void pump_init(void)
{
    nrf_gpio_cfg_output(14);
    nrf_gpio_pin_write(14, 0);
}

void valve_init(void)
{
    // nrf_drv_pwm_config_t const config0 = {
    //     .output_pins =
    //         {
    //             12,                       // channel 0
    //             NRF_DRV_PWM_PIN_NOT_USED, // channel 1
    //             NRF_DRV_PWM_PIN_NOT_USED, // channel 2
    //             NRF_DRV_PWM_PIN_NOT_USED, // channel 3
    //         },
    //     .irq_priority = APP_IRQ_PRIORITY_LOWEST,
    //     .base_clock = NRF_PWM_CLK_1MHz,
    //     .count_mode = NRF_PWM_MODE_UP,
    //     .top_value = 100,
    //     .load_mode = NRF_PWM_LOAD_COMMON,
    //     .step_mode = NRF_PWM_STEP_AUTO,
    // };
    // APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));

    nrf_gpio_cfg_output(12);
    nrf_gpio_pin_write(12, 0);
    // valve_ctl(0);
}

float g_target_pressure_value = 200000; // 目标压力
int g_hold_time = 60;                   // 保压时间 *100ms
int g_deflating_time = 60;              // 间隔时间 *100ms

float pid_cst_kp;
float pid_cst_ki;
float pid_cst_kd;

pid_t work_pid;

void state_machine_init(void)
{
    pid_init(&work_pid, pid_cst_kp, pid_cst_ki, pid_cst_kd, 0, 100);
}

void state_machine_loop(int tick) // s
{
    static int tick_count = 0;
    float pre;
    // uint16_t pwm_s;

    NRF_LOG_INFO("g_work_state:%d", g_work_state);

    switch (g_work_state)
    {
    case WORK_IDLE:
        valve_ctl(0);
        nrf_gpio_pin_write(14, 0);
        tick_count = 0;
        break;

    case WORK_START: // 开始
        valve_ctl(1);
        nrf_gpio_pin_write(14, 1);
        g_work_state = WORK_INFLATING;
        tick_count = 0;
        break;

    case WORK_INFLATING: // 加压
        pre = get_pressure_value();
        if (pre >= g_target_pressure_value || tick_count >= 60)
        {
            g_work_state = WORK_HOLDING;
            nrf_gpio_pin_write(14, 0);
            tick_count = 0;
        }
        else
        {
            tick_count += tick;
        }
        break;

    case WORK_HOLDING: // 保压
        if (tick_count >= g_hold_time)
        {
            tick_count = 0;
            valve_ctl(0);
            g_work_state = WORK_DEFLATING;
        }
        else
            tick_count += tick;
        break;

    case WORK_DEFLATING:
        if (tick_count >= g_deflating_time)
        {
            tick_count = 0;
            g_work_state = WORK_START;
        }
        else
            tick_count += tick;
        break;

    default:
        break;
    }
}
