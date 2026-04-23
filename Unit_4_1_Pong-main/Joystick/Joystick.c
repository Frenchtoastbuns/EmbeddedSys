#include "Joystick.h"

static uint16_t read_adc_channel(Joystick_cfg_t* cfg, uint32_t channel)
{
    cfg->adc_config.Channel = channel;
    HAL_ADC_ConfigChannel(cfg->adc, &cfg->adc_config);

    HAL_ADC_Start(cfg->adc);
    HAL_ADC_PollForConversion(cfg->adc, HAL_MAX_DELAY);
    uint16_t value = (uint16_t)HAL_ADC_GetValue(cfg->adc);
    HAL_ADC_Stop(cfg->adc);

    return value;
}

static int8_t axis_to_direction(int16_t delta, uint16_t deadzone)
{
    if (delta > (int16_t)deadzone) {
        return 1;
    }
    if (delta < -(int16_t)deadzone) {
        return -1;
    }
    return 0;
}

static Direction direction_from_axes(int8_t x_dir, int8_t y_dir)
{
    if (x_dir == 0 && y_dir == 0) {
        return CENTRE;
    }
    if (x_dir == 0 && y_dir < 0) {
        return N;
    }
    if (x_dir > 0 && y_dir < 0) {
        return NE;
    }
    if (x_dir > 0 && y_dir == 0) {
        return E;
    }
    if (x_dir > 0 && y_dir > 0) {
        return SE;
    }
    if (x_dir == 0 && y_dir > 0) {
        return S;
    }
    if (x_dir < 0 && y_dir > 0) {
        return SW;
    }
    if (x_dir < 0 && y_dir == 0) {
        return W;
    }
    return NW;
}

void Joystick_Init(Joystick_cfg_t* cfg)
{
    if (cfg->setup_done != 0u) {
        return;
    }

    HAL_ADCEx_Calibration_Start(cfg->adc, ADC_SINGLE_ENDED);

    cfg->adc_config.Rank = ADC_REGULAR_RANK_1;
    cfg->adc_config.SamplingTime = cfg->sampling_time;
    cfg->adc_config.SingleDiff = ADC_SINGLE_ENDED;
    cfg->adc_config.OffsetNumber = ADC_OFFSET_NONE;
    cfg->adc_config.Offset = 0;

    cfg->setup_done = 1u;
}

void Joystick_Calibrate(Joystick_cfg_t* cfg)
{
    uint32_t x_sum = 0u;
    uint32_t y_sum = 0u;
    const uint8_t sample_count = 50u;

    for (uint8_t i = 0u; i < sample_count; i++) {
        x_sum += read_adc_channel(cfg, cfg->x_channel);
        y_sum += read_adc_channel(cfg, cfg->y_channel);
        HAL_Delay(10u);
    }

    cfg->center_x = (uint16_t)(x_sum / sample_count);
    cfg->center_y = (uint16_t)(y_sum / sample_count);
}

void Joystick_Read(Joystick_cfg_t* cfg, Joystick_t* data)
{
    data->x_raw = read_adc_channel(cfg, cfg->x_channel);
    data->y_raw = read_adc_channel(cfg, cfg->y_channel);

    data->x_delta = (int16_t)data->x_raw - (int16_t)cfg->center_x;
    data->y_delta = (int16_t)data->y_raw - (int16_t)cfg->center_y;

    data->x_dir = axis_to_direction(data->x_delta, cfg->deadzone);
    data->y_dir = axis_to_direction(data->y_delta, cfg->deadzone);
    data->direction = direction_from_axes(data->x_dir, data->y_dir);
}

int8_t Joystick_GetXDirection(const Joystick_t* data)
{
    return data->x_dir;
}

int8_t Joystick_GetYDirection(const Joystick_t* data)
{
    return data->y_dir;
}

Direction Joystick_GetDirection(const Joystick_t* data)
{
    return data->direction;
}
