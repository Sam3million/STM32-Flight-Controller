/*
 * app_tasks.c
 *
 *  Created on: 25/10/2026
 *      Author: samuelrose
 */


#include "cmsis_os.h"
#include "main.h"
#include "mpu6050.h"
#include "sensor_average.h"
#include "app_config.h"
#include "ui_elements.h"
#include <stdio.h>
#include <string.h>
#include "servo.h"
#include "pid.h"
#include "math.h"

#define PID_DIV 10
#define MAX_TILT_DEG 60.0f
#define MAX_SLEW_DEG_S 400.0f
#define OUTPUT_SIGN (+1.0f)
#define ROLL_RATE_SIGN (-1.0f)

extern osMutexId_t sharedMutexHandle;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart3;
extern I2C_HandleTypeDef hi2c1;

typedef struct
{
	uint32_t tMs;
	int32_t rollComp;
	int32_t rollK1;
	int32_t rollK2;
	int32_t pitchComp;
	int32_t pitchK1;
	int32_t pitchK2;
	uint32_t dtUs;
} TelemetrySample;

typedef struct
{
	Attitude attitude;
	SensorAverage average;
	TelemetrySample telemetry;
	float commandRoll;
} SharedState;

static SharedState sharedState;
static Attitude displayAttitude = {0};
static FlightUI ui;

/*
static volatile uint8_t dtLogFull = 0;
#define DT_LOG_SIZE 5000
static uint16_t dtLog[DT_LOG_SIZE];
static uint32_t dtLogCount = 0;

static inline void DtLog_Add(uint32_t us)
{
	if (dtLogCount < DT_LOG_SIZE)
	{
		dtLog[dtLogCount] = (us > 65535u) ? 65535u : (uint16_t)us;
		dtLogCount++;
	}
}

static void DtLog_Dump()
{
	printf("index,dt_us\n");
	for (uint32_t i = 0; i < dtLogCount; i++)
	{
		printf("%lu,%u\n", (unsigned long)i, dtLog[i]);
	}
	printf("END\n");
}
*/

static uint32_t frames = 0, fps = 0, fpsStart = 0;

static uint32_t servoDiv = 0;

static float SweepAngleDeg(uint32_t nowMs, float amplitudeDeg, float periodS)
{
	uint32_t periodMs = (uint32_t)(periodS * 1000.0f);
	float phase = (float)(nowMs % periodMs) / (float)periodMs;
	float tri = (phase < 0.5f) ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
	return amplitudeDeg * tri;
}

void ControlTask_Run(void* argument)
{
	Servo_SetAngleDeg(0);
	osDelay(2000);
	MPU6050_Init(&hi2c1);

	Attitude attComp = {0}, attK1 = {0}, attK2 = {0};
	float3 accel, gyro;
	float temperature;
	float cmdDeg = 0.0f;

	Pid pid;
	Pid_Init(&pid, 1.0f, 5.0f, 0.0f, 20.0f, 20.0f);
	uint32_t pidDiv = 0;

	uint32_t next = osKernelGetTickCount();
	uint32_t lastUs = __HAL_TIM_GET_COUNTER(&htim2);
	uint32_t lastPidUs = lastUs;
	for (;;)
	{
		next += 1;

		/*if (++servoDiv >= 10)
		{
			servoDiv = 0;
			cmdDeg = SweepAngleDeg(osKernelGetTickCount(), 20.0f, 6.0f);
			Servo_SetAngleDeg(cmdDeg);
			//shared.servoCmdDeg = cmdDeg;
		}*/

		MPU6050_GetSensorData(&accel, &temperature, &gyro);

		uint32_t nowUs = __HAL_TIM_GET_COUNTER(&htim2);
		float dt = (float)(nowUs - lastUs) * 0.000001f;
		lastUs = nowUs;

		/*if (!dtLogFull)
		{
			DtLog_Add((uint32_t)(dt * 1e6f));
			if (dtLogCount >= DT_LOG_SIZE) dtLogFull = 1;
		}*/
		// do all 3 filters for comparison:
		MPU6050_ComplementaryFilter(&attComp, accel, gyro, dt);
		MPU6050_Kalman1Filter(&attK1, accel, gyro, dt);
		MPU6050_Kalman2Filter(&attK2, accel, gyro, dt);
		Attitude* plate = &attK2;

		if (++pidDiv >= PID_DIV)
		{
			pidDiv = 0;
			float dtPid = (float)(nowUs - lastPidUs) * 1e-6f;
			lastPidUs = nowUs;

			// safety: invalid estimate or extreme tilt forces a disarm
			if (!isfinite(plate->roll) || fabsf(plate->roll) > MAX_TILT_DEG) mode = SERVO_DISARMED;

			float target = OUTPUT_SIGN * Pid_Update(&pid, 0.0f, plate->roll, ROLL_RATE_SIGN * gyro.x, dtPid);

			float step = MAX_SLEW_DEG_S * dtPid;
			float diff = target - cmdDeg;
			if (diff > step) diff = step;
			if (diff < -step) diff = -step;
			cmdDeg += diff;
			Servo_SetAngleDeg(cmdDeg);
		}

		osMutexAcquire(sharedMutexHandle, osWaitForever);

		//sharedState.attitude = attK2;
		shared.attitude = *plate;
		sharedState.telemetry.tMs = osKernelGetTickCount();
		sharedState.telemetry.rollComp = (int32_t)lroundf(attComp.roll * 100.0f);
		sharedState.telemetry.rollK1 = (int32_t)lroundf(attK1.roll * 100.0f);
		sharedState.telemetry.rollK2 = (int32_t)lroundf(attK2.roll * 100.0f);
		sharedState.telemetry.pitchComp = (int32_t)lroundf(attComp.pitch * 100.0f);
		sharedState.telemetry.pitchK1 = (int32_t)lroundf(attK1.pitch * 100.0f);
		sharedState.telemetry.pitchK2 = (int32_t)lroundf(attK2.pitch * 100.0f);
		sharedState.telemetry.dtUs = (uint32_t)(dt * 1e6f);
		sharedState.commandRoll = cmdDeg;
		SensorAverage_Add(&sharedState.average, accel, gyro, temperature);

		osMutexRelease(sharedMutexHandle);

		// if we ran late, don't try to catch up, just reset the schedule:
		if (osDelayUntil(next) != osOK) next = osKernelGetTickCount();
	}
}

void StartDisplayTask(void* argument)
{
	FlightUI_Init(&ui, &displayAttitude, "  Servo Test");

	float3 accel, gyro;
	float temperature;
	for (;;)
	{
		osMutexAcquire(sharedMutexHandle, osWaitForever);
		displayAttitude = sharedState.attitude;
		SensorAverage_Take(&sharedState.average, &accel, &gyro, &temperature);
		osMutexRelease(sharedMutexHandle);

		FlightUI_Update(&ui, accel, gyro, temperature, fps, sharedState.commandRoll);

		frames++;
		uint32_t now = osKernelGetTickCount();
		if (now - fpsStart >= 1000)
		{
			fps = frames;
			frames = 0;
			fpsStart = now;
		}


		osDelay(50);
	}
}

void StartTelemetryTask(void* argument)
{
	for (;;)
	{
		/*if (dtLogFull)
		{
			DtLog_Dump();
			dtLogFull = 0xFF;
		}
		osDelay(100);
		*/

		char buf[128];
		uint32_t next = osKernelGetTickCount();
		const char *hdr = "t_ms,roll_comp,roll_k1,roll_k2,pitch_comp,pitch_k1,pitch_k2,roll_cmd,dt_us\r\n";
		HAL_UART_Transmit(&huart3, (uint8_t*)hdr, strlen(hdr), 100);

		for (;;)
		{
			next += 10;
			TelemetrySample sample;
			osMutexAcquire(sharedMutexHandle, osWaitForever);
			sample = sharedState.telemetry;
			osMutexRelease(sharedMutexHandle);

			int n = snprintf(buf, sizeof buf, "%lu,%ld,%ld,%ld,%ld,%ld,%ld,%lu\r\n",
							 (unsigned long)sample.tMs,
							 (long)sample.rollComp, (long)sample.rollK1, (long)sample.rollK2,
							 (long)sample.pitchComp, (long)sample.pitchK1, (long)sample.pitchK2,
							 (long)lroundf(sharedState.commandRoll * 100.0f),
							 (unsigned long)sample.dtUs);
			HAL_UART_Transmit(&huart3, (uint8_t*)buf, n, 20);
			osDelayUntil(next);
		}
	}
}
