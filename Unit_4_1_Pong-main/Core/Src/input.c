#include "input.h"

#include "adc.h"
#include "Joystick.h"
#include "main.h"

#define INPUT_ACTION_DEBOUNCE_MS  35u

static Joystick_cfg_t joystick_cfg = {
    .adc = &hadc1,
    .x_channel = ADC_CHANNEL_1,
    .y_channel = ADC_CHANNEL_2,
    .sampling_time = ADC_SAMPLETIME_47CYCLES_5,
    .center_x = JOYSTICK_DEFAULT_CENTER_X,
    .center_y = JOYSTICK_DEFAULT_CENTER_Y,
    .deadzone = JOYSTICK_DEADZONE,
    .setup_done = 0u
};

static Joystick_t joystick_data;
static GameInput_t input_state;
static uint8_t last_action_down = 0u;
static uint32_t last_action_change_tick = 0u;

static uint8_t is_action_button_down(void)
{
    uint8_t btn2_down = (HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin) == GPIO_PIN_RESET);
    uint8_t b1_down = (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET);
    return (uint8_t)(btn2_down || b1_down);
}

void input_init(void)
{
    Joystick_Init(&joystick_cfg);

    input_state.move_x = 0;
    input_state.move_y = 0;
    input_state.action_down = 0u;
    input_state.action_pressed = 0u;

    last_action_down = is_action_button_down();
    last_action_change_tick = HAL_GetTick();
}

void read_input(void)
{
    Joystick_Read(&joystick_cfg, &joystick_data);

    input_state.move_x = Joystick_GetXDirection(&joystick_data);
    input_state.move_y = Joystick_GetYDirection(&joystick_data);
    input_state.action_pressed = 0u;

    uint8_t action_now = is_action_button_down();
    uint32_t now = HAL_GetTick();

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
