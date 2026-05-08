#include "input.h"
#include "adc.h"
#include "main.h"

/*
 * Input module.
 *
 * This file reads the physical joystick/action button and turns them into a
 * small GameInput_t. It does not decide what the player should do in the game;
 * game.c reads the result and applies the rules.
 */

#define INPUT_ACTION_DEBOUNCE_MS      35u
#define INPUT_DIRECTION_CONFIRM_READS 3u
#define INPUT_JOYSTICK_DEADZONE       650u
#define INPUT_ACTION_GPIO_Port        GPIOA
#define INPUT_ACTION_Pin              GPIO_PIN_6
#define JOYSTICK_DEFAULT_CENTER_X     2048u
#define JOYSTICK_DEFAULT_CENTER_Y     2048u

typedef struct {
    ADC_HandleTypeDef* adc;
    uint32_t x_channel;
    uint32_t y_channel;
    uint32_t sampling_time;
    uint16_t center_x;
    uint16_t center_y;
    uint16_t deadzone;
    ADC_ChannelConfTypeDef adc_config;
} InputJoystickConfig_t;

static InputJoystickConfig_t joystick_cfg = {
    .adc = &hadc1,
    /*
     * Joystick 1:
     * X -> PC1 / ADC1_IN2
     * Y -> PC0 / ADC1_IN1
     */
    .x_channel = ADC_CHANNEL_2,
    .y_channel = ADC_CHANNEL_1,
    .sampling_time = ADC_SAMPLETIME_47CYCLES_5,
    .center_x = JOYSTICK_DEFAULT_CENTER_X,
    .center_y = JOYSTICK_DEFAULT_CENTER_Y,
    .deadzone = INPUT_JOYSTICK_DEADZONE
};

static GameInput_t input_state;
static uint8_t last_action_down = 0u;
static uint32_t last_action_change_tick = 0u;
static int8_t filtered_x = 0, filtered_y = 0;
static int8_t pending_x = 0, pending_y = 0;
static uint8_t pending_x_count = 0u, pending_y_count = 0u;
static Direction last_4way_direction = CENTRE;

static uint16_t read_adc(uint32_t channel)
{
    uint16_t val;

    /* ADC1 is shared between the X and Y pins, so the channel is swapped here. */
    joystick_cfg.adc_config.Channel = channel;
    HAL_ADC_ConfigChannel(joystick_cfg.adc, &joystick_cfg.adc_config);
    HAL_ADC_Start(joystick_cfg.adc);
    HAL_ADC_PollForConversion(joystick_cfg.adc, HAL_MAX_DELAY);
    val = (uint16_t)HAL_ADC_GetValue(joystick_cfg.adc);
    HAL_ADC_Stop(joystick_cfg.adc);
    return val;
}

static int8_t raw_axis(int16_t delta)
{
    if (delta > (int16_t)INPUT_JOYSTICK_DEADZONE) {
        return 1;
    }
    if (delta < -(int16_t)INPUT_JOYSTICK_DEADZONE) {
        return -1;
    }
    return 0;
}

static Direction axes_to_4way_direction(int8_t x, int8_t y)
{
    /*
     * The game uses simple 4-way movement. If the stick is diagonal, keep the
     * last axis where possible so the player does not jitter between axes.
     */
    if (x == 0 && y == 0) {
        last_4way_direction = CENTRE;
        return CENTRE;
    }

    if (x != 0 && y != 0) {
        if (last_4way_direction == E || last_4way_direction == W) {
            return (x > 0) ? E : W;
        }
        return (y > 0) ? S : N;
    }

    if (x > 0) {
        last_4way_direction = E;
    } else if (x < 0) {
        last_4way_direction = W;
    } else if (y > 0) {
        last_4way_direction = S;
    } else {
        last_4way_direction = N;
    }

    return last_4way_direction;
}

static int8_t filter_axis(int8_t raw, int8_t* filtered, int8_t* pending, uint8_t* count)
{
    /* A direction has to appear for a few reads before it is accepted. */
    if (raw == *filtered) {
        *pending = raw;
        *count = 0u;
        return *filtered;
    }
    if (raw == 0) {
        *filtered = 0;
        *pending = 0;
        *count = 0u;
        return 0;
    }
    if (raw == *pending) {
        if (*count < INPUT_DIRECTION_CONFIRM_READS) {
            (*count)++;
        }
    } else {
        *pending = raw;
        *count = 1u;
    }
    if (*count >= INPUT_DIRECTION_CONFIRM_READS) {
        *filtered = *pending;
        *count = 0u;
    }
    return *filtered;
}

static uint8_t action_button_down(void)
{
    return (uint8_t)(HAL_GPIO_ReadPin(INPUT_ACTION_GPIO_Port, INPUT_ACTION_Pin) == GPIO_PIN_RESET);
}

static void input_configure_action_gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = INPUT_ACTION_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(INPUT_ACTION_GPIO_Port, &GPIO_InitStruct);
}

void input_init(void)
{
    uint32_t sx = 0u, sy = 0u;

    input_configure_action_gpio();

    HAL_ADCEx_Calibration_Start(joystick_cfg.adc, ADC_SINGLE_ENDED);
    joystick_cfg.adc_config.Rank = ADC_REGULAR_RANK_1;
    joystick_cfg.adc_config.SamplingTime = joystick_cfg.sampling_time;
    joystick_cfg.adc_config.SingleDiff = ADC_SINGLE_ENDED;
    joystick_cfg.adc_config.OffsetNumber = ADC_OFFSET_NONE;
    joystick_cfg.adc_config.Offset = 0;

    /* Average the starting stick position so centre drift is less annoying. */
    for (uint8_t i = 0u; i < 50u; i++) {
        sx += read_adc(joystick_cfg.x_channel);
        sy += read_adc(joystick_cfg.y_channel);
        HAL_Delay(10u);
    }

    joystick_cfg.center_x = (uint16_t)(sx / 50u);
    joystick_cfg.center_y = (uint16_t)(sy / 50u);

    input_state.move_x = 0;
    input_state.move_y = 0;
    input_state.joy1_direction = CENTRE;
    input_state.joy2_direction = CENTRE;
    input_state.action_down = 0u;
    input_state.action_pressed = 0u;
    filtered_x = filtered_y = 0;
    pending_x = pending_y = 0;
    pending_x_count = pending_y_count = 0u;
    last_4way_direction = CENTRE;

    last_action_down = action_button_down();
    last_action_change_tick = HAL_GetTick();
}

void read_input(void)
{
    uint16_t x_raw = read_adc(joystick_cfg.x_channel);
    uint16_t y_raw = read_adc(joystick_cfg.y_channel);
    int8_t x_raw_dir = raw_axis((int16_t)x_raw - (int16_t)joystick_cfg.center_x);
    int8_t y_raw_dir = raw_axis((int16_t)joystick_cfg.center_y - (int16_t)y_raw);
    uint8_t action_now;
    uint32_t now;

    input_state.move_x = filter_axis(x_raw_dir, &filtered_x, &pending_x, &pending_x_count);
    input_state.move_y = filter_axis(y_raw_dir, &filtered_y, &pending_y, &pending_y_count);
    input_state.joy1_direction = axes_to_4way_direction(input_state.move_x, input_state.move_y);
    input_state.joy2_direction = CENTRE;
    input_state.action_pressed = 0u;

    action_now = action_button_down();
    now = HAL_GetTick();

    if (action_now != last_action_down &&
        (uint32_t)(now - last_action_change_tick) >= INPUT_ACTION_DEBOUNCE_MS) {
        last_action_change_tick = now;
        last_action_down = action_now;
        if (action_now != 0u) {
            input_state.action_pressed = 1u;
        }
    }

    input_state.action_down = last_action_down;
}

const GameInput_t* input_get_state(void)
{
    return &input_state;
}
