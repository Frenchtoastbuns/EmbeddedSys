// STM32 board repair game
// =======================
// Main loop used by the demo:
//   read_buttons()
//   play_game()
//   update_extra_stuff()
//   draw_screen()

#include "main.h"
#include "gpio.h"
#include "adc.h"
#include "usart.h"

#include "game.h"
#include "hardware.h"
#include "input.h"
#include "render.h"

#include <stdint.h>
#include <stdio.h>

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

#define GAME_TARGET_FPS          60u
#define GAME_FRAME_TIME_MS       (1000u / GAME_TARGET_FPS)

int _write(int file, char *ptr, int len)
{
    (void)file;

    if (len <= 0) {
        return 0;
    }

    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}

static void setup_board(void)
{
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    /*
     * CubeMX-generated setup first.
     *
     * Current demo pinout:
     *   LCD: SPI2, PB13 SCLK, PB15 MOSI, PB12 CS, PB11 DC, PB2 RST, PB1 BL
     *   Joystick 1: PC1 ADC1_IN2 X, PC0 ADC1_IN1 Y
     *   Action: PA6 input with pull-up
     */
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();

    start_screen();
    start_input();
    start_game();

    /* Optional external hardware is disabled for this final demo. */
    start_extra_stuff();

    printf("Board repair game ready.\r\n");
}

static uint8_t time_for_frame(uint32_t* last_tick)
{
    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - *last_tick) < GAME_FRAME_TIME_MS) {
        return 0u;
    }

    *last_tick = now;
    return 1u;
}

static void game_loop_once(void)
{
    read_buttons();
    play_game();

    /* Kept as a no-op hook so the loop stays simple. */
    update_extra_stuff(get_game());

    draw_screen();
}

int main(void)
{
    uint32_t last_tick;

    setup_board();
    last_tick = HAL_GetTick();

    /*
     * I kept the loop small on purpose:
     *   joystick -> game rules -> hardware -> LCD.
     *
     * HAL_GetTick() paces it without HAL_Delay(), so the joystick does not
     * get stuck waiting for a delay.
     */
    while (1)
    {
        if (time_for_frame(&last_tick) != 0u) {
            game_loop_once();
        }
    }
}

/*
 * CubeMX-generated STM32 functions.
 * I left these alone unless CubeMX needs to regenerate them.
 */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

void PeriphCommonClock_Config(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG | RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_PLLSAI1;

    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK |
                                            RCC_PLLSAI1_ADC1CLK;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif /* USE_FULL_ASSERT */
