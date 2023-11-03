#include "work.h"
#include "pressure.h"
#include "pid.h"

#include "nrf_drv_pwm.h"
#include "nrf_gpio.h"

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_common_t m_seq_values;
static nrf_pwm_sequence_t const m_seq = {
    .values.p_common = &m_seq_values,
    .length = 1,
    .repeats = 0,
    .end_delay = 0,
};

work_state_t g_work_state = WORK_IDLE;

static void pump_update_pwm(int16_t duty_cycle)
{
    if (duty_cycle != 0)
    {
        m_seq_values = duty_cycle;
        nrf_drv_pwm_simple_playback(&m_pwm0, &m_seq, 1, 0);
    }
    else
    {
        nrfx_pwm_stop(&m_pwm0, false);
    }
}

void pump_init(void)
{
    nrf_drv_pwm_config_t const config0 =
        {
            .output_pins =
                {
                    14,                       // channel 0
                    NRF_DRV_PWM_PIN_NOT_USED, // channel 1
                    NRF_DRV_PWM_PIN_NOT_USED, // channel 2
                    NRF_DRV_PWM_PIN_NOT_USED, // channel 3
                },
            .irq_priority = APP_IRQ_PRIORITY_LOWEST,
            .base_clock = NRF_PWM_CLK_1MHz,
            .count_mode = NRF_PWM_MODE_UP,
            .top_value = 100,
            .load_mode = NRF_PWM_LOAD_COMMON,
            .step_mode = NRF_PWM_STEP_AUTO};
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));

    pump_update_pwm(0);
}

void valve_init(void)
{
    nrf_gpio_cfg_output(12);
    nrf_gpio_pin_write(12, 0);
}

float g_target_pressure_value; // 目标压力
int g_hold_time;               // 保压时间 *500ms
int g_deflating_time;          // 间隔时间 *500ms

float pid_cst_kp;
float pid_cst_ki;
float pid_cst_kd;

pid_t work_pid;

uint8_t g_work_start;

void state_machine_init(void)
{
    pid_init(&work_pid, pid_cst_kp, pid_cst_ki, pid_cst_kd, 0, 100);
}

void state_machine_loop(int tick)
{
    if (g_work_start == 0)
        return;

    static int tick_count = 0;
    float pre;
    uint16_t pwm_s;

    switch (g_work_state)
    {
    case WORK_IDLE:
        break;
    case WORK_START:
        nrf_gpio_pin_write(12, 1);
        pump_update_pwm(50);
        break;
    case WORK_INFLATING:
        pre = get_pressure_value();
        if (pre >= g_target_pressure_value)
        {
            g_work_state = WORK_HOLDING;
            pump_update_pwm(0);
        }
        break;
    case WORK_HOLDING:
        pre = get_pressure_value();
        pwm_s = (uint8_t)pid_update(&work_pid, pre, g_target_pressure_value);
        pump_update_pwm(pwm_s);
        if (tick_count >= g_hold_time)
        {
            tick_count = 0;
            g_work_state = WORK_DEFLATING;
        }
        else
            tick_count++;
        break;
    case WORK_DEFLATING:
        if (tick_count >= g_deflating_time)
        {
            tick_count = 0;
            g_work_state = WORK_START;
        }
        else
            tick_count++;
        break;
    default:
        break;
    }
}
