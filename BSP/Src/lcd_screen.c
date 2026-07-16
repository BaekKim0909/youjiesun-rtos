//
// Created by 74222 on 2026/7/14.
//

#include "lcd_screen.h"
#include "main.h"
void lcd_screen_set_bg_brightness(uint8_t brightness)
{
	if (brightness <= 0)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
	}
	else if (brightness >= 1 && brightness <= 4)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 24 + 25 * (brightness - 1));
	}
	else if (brightness > 4)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 99);
	}
}
