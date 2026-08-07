//
// Created by 74222 on 2026/8/7.
//

#include "../Inc/bsp_matrix_key.h"
#include "main.h"

/*
 * 4 * 5 矩阵键盘
 */

#define KEY_ROW_COUNT 4U
#define KEY_COL_COUNT 5U

typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
}Key_GPIO_t;

/* 行引脚：扫描时每次只将当前行拉低，其余行保持高电平。 */
static const key_gpio_t row_gpios[KEY_ROW_COUNT] = {
    {KB_R1_GPIO_Port, KB_R1_Pin},
    {KB_R2_GPIO_Port, KB_R2_Pin},
    {KB_R3_GPIO_Port, KB_R3_Pin},
    {KB_R4_GPIO_Port, KB_R4_Pin}};

/* 列引脚：硬件配置为上拉输入，按键按下时被当前低电平行拉低。 */
static const key_gpio_t col_gpios[KEY_COL_COUNT] = {
    {KB_C1_GPIO_Port, KB_C1_Pin},
    {KB_C2_GPIO_Port, KB_C2_Pin},
    {KB_C3_GPIO_Port, KB_C3_Pin},
    {KB_C4_GPIO_Port, KB_C4_Pin},
    {KB_C5_GPIO_Port, KB_C5_Pin}};

static void key_set_all_rows(GPIO_PinState state)
{
    for (uint8_t row = 0U; row < KEY_ROW_COUNT; row++)
    {
        HAL_GPIO_WritePin(row_gpios[row].port, row_gpios[row].pin, state);
    }
}