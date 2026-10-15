/*
 * ui_elements.h
 *
 *  Created on: 13/10/2026
 *      Author: samuelrose
 */

#ifndef INC_UI_ELEMENTS_H_
#define INC_UI_ELEMENTS_H_

#include <stdint.h>
#include "linalg.h"
#include "font.h"
#include "mpu6050.h"

#define TEXT_MAX_CHARS 64

typedef struct
{
	ushort2 pos;
	uint8_t scale;
	const Font* font;
	uint16_t textColor;
	uint16_t backgroundColor;
	uint8_t length;
	char text[TEXT_MAX_CHARS + 1];
	char currentlyVisible[TEXT_MAX_CHARS];
	uint8_t valid;
} Text;

typedef struct
{
	Attitude* attitude;
	ushort2 center;
	uint16_t radius;
} AttitudeDisplay;

typedef struct {
	Text accText, gyroText, tempText, angleText;
	AttitudeDisplay horizon;
} FlightUI;


void Text_Init(Text* text, uint16_t x, uint16_t y, uint8_t scale, uint8_t maxLength, const Font* font, uint16_t textColor, uint16_t backgroundColor);
void Text_SetText(Text* text, const char* s);
void Text_Draw(Text* text);

void AttitudeDisplay_Init(AttitudeDisplay* display, Attitude* attitude, uint16_t x, uint16_t y, uint16_t radius);
void AttitudeDisplay_Draw(AttitudeDisplay* display);

void FlightUI_Init(FlightUI* ui, const Attitude* attitude, const char* title);
void FlightUI_Update(FlightUI* ui, float3 acc, float3 gyro, float temp);

#endif /* INC_UI_ELEMENTS_H_ */
