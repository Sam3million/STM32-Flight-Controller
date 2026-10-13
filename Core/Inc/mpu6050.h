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
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_WHO_AM_I 0x75

void MPU6050_Init(I2C_HandleTypeDef* hi2c);

uint8_t MPU6050_ReadByte(uint16_t address);

void MPU6050_ReadData(uint16_t address, uint8_t* buffer, uint16_t length);

void MPU6050_WriteByte(uint16_t address, uint8_t value);

void MPU6050_WriteData(uint16_t address, uint8_t* buffer, uint16_t length);

void MPU6050_GetSensorData(float3* acceleration, float* temp, float3* gyro);

#endif /* INC_MPU6050_H_ */
