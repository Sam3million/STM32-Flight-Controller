/*
 * util.h
 *
 *  Created on: 11/10/2026
 *      Author: samuelrose
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

static inline int MinInt(int a, int b)
{
	return (a < b) ? a : b;
}

static inline int MaxInt(int a, int b)
{
	return (a > b) ? a : b;
}

static inline void SwapInt(int* a, int* b)
{
	int t = *a; *a = *b; *b = t;
}

static inline void SwapUInt16(uint16_t* a, uint16_t* b)
{
	uint16_t t = *a; *a = *b; *b = t;
}

#endif /* INC_UTIL_H_ */
