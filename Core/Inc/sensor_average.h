/*
 * sensor_average.h
 *
 *  Created on: 25/10/2026
 *      Author: samuelrose
 */

#ifndef INC_SENSOR_AVERAGE_H_
#define INC_SENSOR_AVERAGE_H_

#include "linalg.h"

typedef struct
{
	float3 acc, gyro;
	float temp;
	int count;
} SensorAverage;

void SensorAverage_Add(SensorAverage* a, float3 acc, float3 gyro, float temp);

void SensorAverage_Take(SensorAverage* a, float3* acc, float3* gyro, float* temp);

#endif /* INC_SENSOR_AVERAGE_H_ */
