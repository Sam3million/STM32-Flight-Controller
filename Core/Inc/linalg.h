/*
 * linalg.h
 *
 *  Created on: 12/10/2026
 *      Author: samuelrose
 */

#ifndef INC_LINALG_H_
#define INC_LINALG_H_

#define RAD2DEG 57.2957795f
#define DEG2RAD 0.0174532925f

typedef struct
{
	uint16_t x, y;
} ushort2;

typedef struct
{
	int16_t x, y, z;
} short3;

typedef struct
{
	float x, y;
} float2;

typedef struct
{
	float x, y, z;
} float3;

static void float3_scale(float3* v, float s)
{
	v->x *= s;
	v->y *= s;
	v->z *= s;
}

#endif /* INC_LINALG_H_ */
