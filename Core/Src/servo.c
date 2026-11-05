/*
 * servo.c
 *
 *  Created on: 28/10/2026
 *      Author: samuelrose
 */
#include "servo.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;

void Servo_SetAngleDeg(float deg)
{
	deg -= 7.5f;
	const float usPerDeg = 12.5;
	float microSeconds = 1500 + deg * usPerDeg;
	if (microSeconds < 900) microSeconds = 900;
	if (microSeconds > 2100) microSeconds = 2100;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, microSeconds);
}
