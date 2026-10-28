/*
 * sensor_average.c
 *
 *  Created on: 25/10/2026
 *      Author: samuelrose
 */
#include "sensor_average.h"

void SensorAverage_Add(SensorAverage* a, float3 acc, float3 gyro, float temp)
{
	a->acc.x += acc.x;
	a->acc.y += acc.y;
	a->acc.z += acc.z;
	a->gyro.x += gyro.x;
	a->gyro.y += gyro.y;
	a->gyro.z += gyro.z;
	a->temp += temp;
	a->count++;
}

void SensorAverage_Take(SensorAverage* a, float3* acc, float3* gyro, float* temp)
{
	float inv = (a->count > 0) ? 1.0f / a->count : 0.0f;
	acc->x = a->acc.x * inv;
	acc->y = a->acc.y * inv;
	acc->z = a->acc.z * inv;
	gyro->x = a->gyro.x * inv;
	gyro->y = a->gyro.y * inv;
	gyro->z = a->gyro.z * inv;
	*temp = a->temp * inv;
	*a = (SensorAverage){0};
}
