#include "nrf_drv_twi.h"

static volatile bool m_xfer_done = false;

/* TWI instance ID. */
#define TWI_INSTANCE_ID 0
#define GZP6887D_ADDR (0x6DU)

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

float measure_temp;
float measure_pres;

static void sensor_start(void)
{
    uint32_t err_code = NRF_SUCCESS;
    uint8_t reg[2] = {0x30, 0x0A}; // 0x30寄存器写入0x0A
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, GZP6887D_ADDR, reg, sizeof(reg), false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false)
    {
    }
}

static uint8_t sensor_read(uint8_t reg)
{
    uint8_t read_byte;
    uint32_t err_code = NRF_SUCCESS;

    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, GZP6887D_ADDR, &reg, 1, false);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false)
    {
    }
    m_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, GZP6887D_ADDR, &read_byte, 1);
    APP_ERROR_CHECK(err_code);
    while (m_xfer_done == false)
    {
    }
    return read_byte;
}

void pressure_measure(void)
{
    sensor_start();

    uint8_t busy_flag = 0x08;
    uint8_t temp_msb;
    uint8_t temp_lsb;

    uint8_t pres_msb;
    uint8_t pres_csb;
    uint8_t pres_lsb;

    while (busy_flag & 0x08)
    {
        busy_flag = sensor_read(0x30);
    }

    temp_msb = sensor_read(0x09);
    temp_lsb = sensor_read(0x0A);

    measure_temp = (float)(temp_msb << 8 | temp_lsb) / 256;

    // printf("temp is %f\n\r", measure_temp);

    pres_msb = sensor_read(0x06);
    pres_csb = sensor_read(0x07);
    pres_lsb = sensor_read(0x08);

    measure_pres = (float)(pres_msb << 16 | pres_csb << 8 | pres_lsb) / 128;
    // printf("pres is %f\n\r", measure_pres);
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
    switch (p_event->type)
    {
    case NRF_DRV_TWI_EVT_DONE:
        if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
        {
        }
        m_xfer_done = true;
        break;
    default:
        break;
    }
}

/**
 * @brief UART initialization.
 */
void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_pressure_config = {
        .scl = 10,
        .sda = 9,
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false};

    err_code = nrf_drv_twi_init(&m_twi, &twi_pressure_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

float get_pressure_value(void)
{
    return measure_pres;
}

float get_tempture_value(void)
{
    return measure_temp;
}
