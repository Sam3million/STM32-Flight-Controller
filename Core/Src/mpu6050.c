/*
 * mpu6050.c
 *
 *  Created on: 12/10/2026
 *      Author: samuelrose
 */


#include "mpu6050.h"

I2C_HandleTypeDef* imuI2C;
float gyroScale;
float accelScale;
float3 accelOffset;
float3 gyroOffset;

static Kalman1Filter rollFilter;
static Kalman1Filter pitchFilter;

static Kalman2Filter rollFilter2;
static Kalman2Filter pitchFilter2;

static void MPU6050_Kalman1_Init(Kalman1Filter* k, float processNoise, float measurementNoise);
static void MPU6050_Kalman2_Init(Kalman2Filter* k, float qAngle, float qBias, float rMeasure);

void MPU6050_Init(I2C_HandleTypeDef* hi2c, Attitude* attitude)
{
	imuI2C = hi2c;
	MPU6050_WriteByte(MPU6050_PWR_MGMT_1, 0);

	HAL_Delay(100);

	uint8_t sel[2];
	MPU6050_ReadData(MPU6050_GYRO_CONFIG, sel, 2);
	sel[0] = (sel[0] >> 3) & 3;
	sel[1] = (sel[1] >> 3) & 3;
	if (sel[0] == 0)
	{
		gyroScale = 1.0f / 131.0f;
	}
	else if (sel[0] == 1)
	{
		gyroScale = 1.0f / 65.5f;
	}
	else if (sel[0] == 2)
	{
		gyroScale = 1.0f / 32.8f;
	}
	else
	{
		gyroScale = 1.0f / 16.4f;
	}
	if (sel[1] == 0)
	{
		accelScale = 1.0f / 16384.0f;
	}
	else if (sel[1] == 1)
	{
		accelScale = 1.0f / 8192.0f;
	}
	else if (sel[1] == 2)
	{
		accelScale = 1.0f / 4096.0f;
	}
	else
	{
		accelScale = 1.0f / 2048.0f;
	}

	// Enable on-chip low pass filter:
	MPU6050_WriteByte(MPU6050_CONFIG, 3);

	MPU6050_WriteByte(MPU6050_SMPLRT_DIV, 0);

	// Initialize kalman filters:
	const float measurementNoise = 0.05f;
	const float tauSeconds = 0.5f;
	const float processNoise = measurementNoise / (tauSeconds * tauSeconds);
	MPU6050_Kalman1_Init(&rollFilter, processNoise, measurementNoise);
	MPU6050_Kalman1_Init(&pitchFilter, processNoise, measurementNoise);

	MPU6050_Kalman2_Init(&rollFilter2, 0.002f, 0.0001f, 0.1f);
	MPU6050_Kalman2_Init(&pitchFilter2, 0.002f, 0.0001f, 0.1f);

	// Calibrate:
	uint32_t startTime = HAL_GetTick();
	float3 accel;
	float temp;
	float3 gyro;

	gyroOffset.x = 0.0f;
	gyroOffset.y = 0.0f;
	gyroOffset.z = 0.0f;
	accelOffset.x = 0.0f;
	accelOffset.y = 0.0f;
	accelOffset.z = 0.0f;

	float3 tempGyroOffset = {0.0f, 0.0f, 0.0f};
	float3 tempAccelOffset = {0.0f, 0.0f, 0.0f};

	int samples = 0;
	while (HAL_GetTick() - startTime < 100)
	{
		MPU6050_GetSensorData(&accel, &temp, &gyro);
		tempGyroOffset.x += gyro.x;
		tempGyroOffset.y += gyro.y;
		tempGyroOffset.z += gyro.z;

		tempAccelOffset.x += accel.x;
		tempAccelOffset.y += accel.y;
		tempAccelOffset.z += accel.z;

		samples++;
	}
	float inv = 1.0f / samples;
	float3_scale(&tempGyroOffset, inv);
	gyroOffset = tempGyroOffset;

	float3_scale(&tempAccelOffset, inv);
	tempAccelOffset.z -= 1;
	accelOffset = tempAccelOffset;

	attitude->roll = 0.0f;
	attitude->pitch = 0.0f;
	attitude->initialized = 0;
}

uint8_t MPU6050_ReadByte(uint16_t address)
{
	uint8_t data;
	HAL_I2C_Mem_Read(imuI2C, MPU6050_I2C_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	return data;
}

void MPU6050_ReadData(uint16_t address, uint8_t* buffer, uint16_t length)
{
	HAL_I2C_Mem_Read(imuI2C, MPU6050_I2C_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

void MPU6050_WriteByte(uint16_t address, uint8_t value)
{
	MPU6050_WriteData(address, &value, 1);
}

void MPU6050_WriteData(uint16_t address, uint8_t* buffer, uint16_t length)
{
	HAL_I2C_Mem_Write(imuI2C, MPU6050_I2C_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

void MPU6050_GetSensorData(float3* accel, float* temp, float3* gyro)
{
	uint8_t raw[14];
	MPU6050_ReadData(MPU6050_ACCEL_XOUT_H, raw, 14);

	int16_t ax = (int16_t)(((uint16_t)raw[0] << 8) | raw[1]);
	int16_t ay = (int16_t)(((uint16_t)raw[2] << 8) | raw[3]);
	int16_t az = (int16_t)(((uint16_t)raw[4] << 8) | raw[5]);
	int16_t t  = (int16_t)(((uint16_t)raw[6] << 8) | raw[7]);
	int16_t gx = (int16_t)(((uint16_t)raw[8] << 8) | raw[9]);
	int16_t gy = (int16_t)(((uint16_t)raw[10] << 8) | raw[11]);
	int16_t gz = (int16_t)(((uint16_t)raw[12] << 8) | raw[13]);

	accel->x = ax * accelScale - accelOffset.x;
	accel->y = ay * accelScale - accelOffset.y;
	accel->z = az * accelScale - accelOffset.z;
	*temp   = t / 340.0f + 36.53f;
	gyro->x  = gx * gyroScale - gyroOffset.x;
	gyro->y  = gy * gyroScale - gyroOffset.y;
	gyro->z  = gz * gyroScale - gyroOffset.z;
}

float2 MPU6050_AccelAttitude(float3 accel)
{
	float2 accelAttitude;
	accelAttitude.x = atan2f(-accel.y, accel.z) * RAD2DEG;
	accelAttitude.y = atan2f(-accel.x, sqrt(accel.y * accel.y + accel.z * accel.z)) * RAD2DEG;

	return accelAttitude;
}

void MPU6050_ComplementaryFilter(Attitude* attitude, float3 accel, float3 gyro, float dt)
{
	float2 accelAttitude = MPU6050_AccelAttitude(accel);

	if (!attitude->initialized)
	{
		attitude->roll  = accelAttitude.x;
		attitude->pitch = accelAttitude.y;
		attitude->initialized = 1;
		return;
	}

	const float tau = 0.5f;
	const float alpha = tau / (tau + dt);

	attitude->roll = alpha * (attitude->roll - gyro.x * dt) + (1.0f - alpha) * accelAttitude.x;
	attitude->pitch = alpha * (attitude->pitch + gyro.y * dt) + (1.0f - alpha) * accelAttitude.y;
}

static void MPU6050_Kalman1_Init(Kalman1Filter* k, float processNoise, float measurementNoise)
{
	k->q = processNoise;
	k->r = measurementNoise;
	k->x = 0.0f;
	k->p = 1.0f;
}

static void MPU6050_UpdateKalman1Filter(Kalman1Filter* kalman, float accelMeasurement, float gyroRate, float dt)
{
	kalman->x += gyroRate * dt; // integrate gyro and add to x
	kalman->p += kalman->q * dt * dt; // add process noise to our error
	kalman->k = kalman->p / (kalman->p + kalman->r);
	kalman->x += kalman->k * (accelMeasurement - kalman->x);
	kalman->p *= (1.0f - kalman->k);
}

void MPU6050_Kalman1Filter(Attitude* attitude, float3 accel, float3 gyro, float dt)
{
	float2 accelAttitude = MPU6050_AccelAttitude(accel);
	MPU6050_UpdateKalman1Filter(&rollFilter, accelAttitude.x, -gyro.x, dt);
	MPU6050_UpdateKalman1Filter(&pitchFilter, accelAttitude.y, gyro.y, dt);
	attitude->roll = rollFilter.x;
	attitude->pitch = pitchFilter.x;
}

static void MPU6050_Kalman2_Init(Kalman2Filter* k, float qAngle, float qBias, float rMeasure)
{
	k->angle = 0.0f;
	k->bias  = 0.0f;
	k->P[0][0] = rMeasure;
	k->P[0][1] = 0.0f;
	k->P[1][0] = 0.0f;
	k->P[1][1] = 1.0f;
	k->qAngle = qAngle;
	k->qBias = qBias;
	k->rMeasure = rMeasure;
	k->initialized = 0;
}

static void MPU6050_UpdateKalman2Filter(Kalman2Filter* k, float accAngle, float gyroRate, float dt)
{
	if (!k->initialized)
	{
		k->angle = accAngle;
		k->initialized = 1;
		return;
	}

	// integrate the gyro with the current bias removed
	k->angle += dt * (gyroRate - k->bias);
	k->P[0][0] += dt * (dt * k->P[1][1] - k->P[0][1] - k->P[1][0] + k->qAngle);
	k->P[0][1] -= dt * k->P[1][1];
	k->P[1][0] -= dt * k->P[1][1];
	k->P[1][1] += k->qBias * dt;

	// correct both the angle and the bias from the accelerometer
	float s  = k->P[0][0] + k->rMeasure;
	float k0 = k->P[0][0] / s;
	float k1 = k->P[1][0] / s;
	float y  = accAngle - k->angle;
	k->angle += k0 * y;
	k->bias  += k1 * y;

	float p00 = k->P[0][0];
	float p01 = k->P[0][1];
	k->P[0][0] -= k0 * p00;
	k->P[0][1] -= k0 * p01;
	k->P[1][0] -= k1 * p00;
	k->P[1][1] -= k1 * p01;
}

void MPU6050_Kalman2Filter(Attitude* attitude, float3 accel, float3 gyro, float dt)
{
	float2 accelAttitude = MPU6050_AccelAttitude(accel);
	MPU6050_UpdateKalman2Filter(&rollFilter2, accelAttitude.x, -gyro.x, dt);
	MPU6050_UpdateKalman2Filter(&pitchFilter2, accelAttitude.y, gyro.y, dt);
	attitude->roll = rollFilter2.angle;
	attitude->pitch = pitchFilter2.angle;
}

