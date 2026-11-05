/*
 * pid.c
 *
 *  Created on: 5/11/2026
 *      Author: samuelrose
 */


#include "pid.h"

void Pid_Init(Pid* p, float kp, float ki, float kd, float integLimit, float outLimit)
{
	p->kp = kp;
	p->ki = ki;
	p->kd = kd;
	p->integLimit = integLimit;
	p->outLimit = outLimit;
	p->integ = 0.0f;
}

void Pid_Reset(Pid* p)
{
	p->integ = 0.0f;
}

float Pid_Update(Pid* p, float setpoint, float measurement, float rate, float dt)
{
	if (dt <= 0.0f || dt > 0.1f) dt = 0.0f;

	float err = setpoint - measurement;

	p->integ += err * dt;
	if (p->ki > 0.0f)
	{
		float lim = p->integLimit / p->ki;
		if (p->integ >  lim) p->integ =  lim;
		if (p->integ < -lim) p->integ = -lim;
	}

	float out = p->kp * err + p->ki * p->integ - p->kd * rate;
	if (out > p->outLimit) out = p->outLimit;
	if (out < -p->outLimit) out = -p->outLimit;
	return out;
}
