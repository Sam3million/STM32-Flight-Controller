/*
 * mpu6050.h
 *
 *  Created on: 12/10/2026
 *      Author: samuelrose
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "main.h"
#include "linalg.h"

#define MPU6050_I2C_ADDRESS (0x68 << 1)
#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1A
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_WHO_AM_I 0x75

typedef struct
{
	float roll, pitch;
	uint8_t initialized;
} Attitude;

// 1-state kalman filter
typedef struct
{
	float q; // process noise
	float r; // measurement noise
	float x; // state estimate
	float p; // estimation error
	float k; // kalman gain
} Kalman1Filter;

// 2-state kalman filter
typedef struct
{
	float angle;
	float bias; 	// gyro bias
	float P[2][2]; 	// error covariance
	float qAngle; 	// process noise on the angle
	float qBias; 	// process noise on the bias
	float rMeasure; // accelerometer angle variance
	uint8_t initialized;
} Kalman2Filter;

void MPU6050_Init(I2C_HandleTypeDef* hi2c, Attitude* attitude);

uint8_t MPU6050_ReadByte(uint16_t address);

void MPU6050_ReadData(uint16_t address, uint8_t* buffer, uint16_t length);

void MPU6050_WriteByte(uint16_t address, uint8_t value);

void MPU6050_WriteData(uint16_t address, uint8_t* buffer, uint16_t length);

void MPU6050_GetSensorData(float3* acceleration, float* temp, float3* gyro);

float2 MPU6050_AccelAttitude(float3 accel);

void MPU6050_ComplementaryFilter(Attitude* lastAttitude, float3 accel, float3 gyro, float dt);

void MPU6050_Kalman1Filter(Attitude* attitude, float3 accel, float3 gyro, float dt);

void MPU6050_Kalman2Filter(Attitude* attitude, float3 accel, float3 gyro, float dt);

#endif /* INC_MPU6050_H_ */
