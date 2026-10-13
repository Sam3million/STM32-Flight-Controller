/*
 * util.h
 *
 *  Created on: 11/10/2026
 *      Author: samuelrose
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include <stdio.h>

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

// Writes e.g. "-12.34" into out. decimals: 0..6
static void FormatFixed(char *out, size_t n, float v, uint8_t decimals)
{
	int32_t scale = 1;
	for (uint8_t i = 0; i < decimals; i++) scale *= 10;

	int32_t scaled = (int32_t)(v * scale + (v >= 0.0f ? 0.5f : -0.5f));   // round to nearest
	const char *sign = "";
	if (scaled < 0) { sign = "-"; scaled = -scaled; }

	int32_t whole = scaled / scale;
	int32_t frac  = scaled % scale;

	char fracStr[8];
	for (int i = decimals - 1; i >= 0; i--) { fracStr[i] = '0' + (frac % 10); frac /= 10; }
	fracStr[decimals] = '\0';

	if (decimals == 0) snprintf(out, n, "%s%ld", sign, (long)whole);
	else               snprintf(out, n, "%s%ld.%s", sign, (long)whole, fracStr);
}

#endif /* INC_UTIL_H_ */
