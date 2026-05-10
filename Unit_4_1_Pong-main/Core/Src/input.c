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
} StickSetup_t;

static StickSetup_t stick = {
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

static GameInput_t input;
static uint8_t old_button = 0u;
static uint32_t button_time = 0u;
static int8_t good_x = 0, good_y = 0;
static int8_t maybe_x = 0, maybe_y = 0;
static uint8_t maybe_x_count = 0u, maybe_y_count = 0u;
static Direction last_dir = CENTRE;

/* reads one ADC channel from the joystick */
static uint16_t read_stick(uint32_t channel)
{
    uint16_t val;

    /* ADC1 is shared between the X and Y pins, so the channel is swapped here. */
    stick.adc_config.Channel = channel;
    HAL_ADC_ConfigChannel(stick.adc, &stick.adc_config);
    HAL_ADC_Start(stick.adc);
    HAL_ADC_PollForConversion(stick.adc, HAL_MAX_DELAY);
    val = (uint16_t)HAL_ADC_GetValue(stick.adc);
    HAL_ADC_Stop(stick.adc);
    return val;
}

/* turns an ADC difference into -1, 0, or 1 */
static int8_t stick_dir(int16_t delta)
{
    if (delta > (int16_t)INPUT_JOYSTICK_DEADZONE) {
        return 1;
    }
    if (delta < -(int16_t)INPUT_JOYSTICK_DEADZONE) {
        return -1;
    }
    return 0;
}

/* turns the x/y stick values into one direction */
static Direction make_stick_dir(int8_t x, int8_t y)
{
    /*
     * The game uses simple 4-way movement. If the stick is diagonal, keep the
     * last axis where possible so the player does not jitter between axes.
     */
    if (x == 0 && y == 0) {
        last_dir = CENTRE;
        return CENTRE;
    }

    if (x != 0 && y != 0) {
        if (last_dir == E || last_dir == W) {
            return (x > 0) ? E : W;
        }
        return (y > 0) ? S : N;
    }

    if (x > 0) {
        last_dir = E;
    } else if (x < 0) {
        last_dir = W;
    } else if (y > 0) {
        last_dir = S;
    } else {
        last_dir = N;
    }

    return last_dir;
}

/* waits for a direction to be steady before using it */
static int8_t smooth_stick(int8_t raw, int8_t* filtered, int8_t* pending, uint8_t* count)
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

/* reads the action button pin */
static uint8_t button_is_down(void)
{
    return (uint8_t)(HAL_GPIO_ReadPin(INPUT_ACTION_GPIO_Port, INPUT_ACTION_Pin) == GPIO_PIN_RESET);
}

/* sets up the action button pin as input pull-up */
static void setup_button(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = INPUT_ACTION_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(INPUT_ACTION_GPIO_Port, &GPIO_InitStruct);
}

/* starts the joystick and button input code */
void start_input(void)
{
    uint32_t sx = 0u, sy = 0u;

    setup_button();

    HAL_ADCEx_Calibration_Start(stick.adc, ADC_SINGLE_ENDED);
    stick.adc_config.Rank = ADC_REGULAR_RANK_1;
    stick.adc_config.SamplingTime = stick.sampling_time;
    stick.adc_config.SingleDiff = ADC_SINGLE_ENDED;
    stick.adc_config.OffsetNumber = ADC_OFFSET_NONE;
    stick.adc_config.Offset = 0;

    /* Average the starting stick position so centre drift is less annoying. */
    for (uint8_t i = 0u; i < 50u; i++) {
        sx += read_stick(stick.x_channel);
        sy += read_stick(stick.y_channel);
        HAL_Delay(10u);
    }

    stick.center_x = (uint16_t)(sx / 50u);
    stick.center_y = (uint16_t)(sy / 50u);

    input.move_x = 0;
    input.move_y = 0;
    input.joy1_direction = CENTRE;
    input.joy2_direction = CENTRE;
    input.action_down = 0u;
    input.action_pressed = 0u;
    good_x = good_y = 0;
    maybe_x = maybe_y = 0;
    maybe_x_count = maybe_y_count = 0u;
    last_dir = CENTRE;

    old_button = button_is_down();
    button_time = HAL_GetTick();
}

/* updates the input state for this frame */
void read_buttons(void)
{
    uint16_t x_raw = read_stick(stick.x_channel);
    uint16_t y_raw = read_stick(stick.y_channel);
    int8_t x_raw_dir = stick_dir((int16_t)x_raw - (int16_t)stick.center_x);
    int8_t y_raw_dir = stick_dir((int16_t)stick.center_y - (int16_t)y_raw);
    uint8_t action_now;
    uint32_t now;

    input.move_x = smooth_stick(x_raw_dir, &good_x, &maybe_x, &maybe_x_count);
    input.move_y = smooth_stick(y_raw_dir, &good_y, &maybe_y, &maybe_y_count);
    input.joy1_direction = make_stick_dir(input.move_x, input.move_y);
    input.joy2_direction = CENTRE;
    input.action_pressed = 0u;

    action_now = button_is_down();
    now = HAL_GetTick();

    if (action_now != old_button &&
        (uint32_t)(now - button_time) >= INPUT_ACTION_DEBOUNCE_MS) {
        button_time = now;
        old_button = action_now;
        if (action_now != 0u) {
            input.action_pressed = 1u;
        }
    }

    input.action_down = old_button;
}

/* gives game.c the latest input values */
const GameInput_t* get_input(void)
{
    return &input;
}
