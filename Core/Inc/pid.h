/*
 * pid.h
 *
 *  Created on: 5/11/2026
 *      Author: samuelrose
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct
{
	float kp, ki, kd;
	float integ;
	float integLimit;
	float outLimit;
} Pid;

void Pid_Init(Pid* p, float kp, float ki, float kd, float integLimit, float outLimit);
void Pid_Reset(Pid* p);
float Pid_Update(Pid* p, float setpoint, float measurement, float rate, float dt);


#endif /* INC_PID_H_ */
