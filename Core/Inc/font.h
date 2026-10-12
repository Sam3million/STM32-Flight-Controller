/*
 * font.h
 *
 *  Created on: 11/10/2026
 *      Author: samuelrose
 */

#ifndef INC_FONT_H_
#define INC_FONT_H_

#include <stdint.h>

typedef struct
{
	const uint8_t *data;   // flat glyph bitmap data
	uint8_t width;
	uint8_t height;
	uint8_t firstChar;     // ASCII code of the first glyph in the table
	uint8_t numChars;      // number of glyphs
	uint8_t msbFirst;      // 1: leftmost pixel is bit 7, 0: leftmost pixel is bit 0
} Font;

#endif /* INC_FONT_H_ */
