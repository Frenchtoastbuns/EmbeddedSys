#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include "main.h"

typedef enum {
    CENTRE = 0,
    N,
    NE,
    E,
    SE,
    S,
    SW,
    W,
    NW
} Direction;

#define JOYSTICK_DEFAULT_CENTER_X 2048u
#define JOYSTICK_DEFAULT_CENTER_Y 2048u
#define JOYSTICK_DEADZONE         200u

typedef struct {
    ADC_HandleTypeDef* adc;
    uint32_t x_channel;
    uint32_t y_channel;
    uint32_t sampling_time;
    uint16_t center_x;
    uint16_t center_y;
    uint16_t deadzone;
    uint8_t setup_done;
    ADC_ChannelConfTypeDef adc_config;
} Joystick_cfg_t;

typedef struct {
    uint16_t x_raw;
    uint16_t y_raw;
    int16_t x_delta;
    int16_t y_delta;
    int8_t x_dir;
    int8_t y_dir;
    Direction direction;
} Joystick_t;

void Joystick_Init(Joystick_cfg_t* cfg);
void Joystick_Calibrate(Joystick_cfg_t* cfg);
void Joystick_Read(Joystick_cfg_t* cfg, Joystick_t* data);
int8_t Joystick_GetXDirection(const Joystick_t* data);
int8_t Joystick_GetYDirection(const Joystick_t* data);
Direction Joystick_GetDirection(const Joystick_t* data);

#endif /* JOYSTICK_H */
