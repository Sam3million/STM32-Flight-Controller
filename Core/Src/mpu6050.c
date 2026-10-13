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

void MPU6050_Init(I2C_HandleTypeDef* hi2c)
{
	imuI2C = hi2c;
	MPU6050_WriteByte(MPU6050_PWR_MGMT_1, 0);
	uint8_t sel[2];
	MPU6050_ReadData(MPU6050_GYRO_CONFIG, sel, 2);
	sel[0] = (sel[0] >> 3) & 3;
	sel[1] = (sel[1] >> 3) & 3;
	if (sel[0] == 0)
	{
		gyroScale = 1.0f / 131.0f;
	}
	else if (sel[1] == 1)
	{
		gyroScale = 1.0f / 65.5f;
	}
	else if (sel[2] == 2)
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

	accel->x = ax * accelScale;
	accel->y = ay * accelScale;
	accel->z = az * accelScale;
	*temp   = t / 340.0f + 36.53f;
	gyro->x  = gx * gyroScale;
	gyro->y  = gy * gyroScale;
	gyro->z  = gz * gyroScale;
}
