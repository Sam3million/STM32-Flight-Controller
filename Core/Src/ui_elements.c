/*
 * ui_elements.c
 *
 *  Created on: 13/10/2026
 *      Author: samuelrose
 */


#include "ui_elements.h"
#include "ili9341.h"
#include "spleen_fonts.h"
#include "util.h"
#include <string.h>

void Text_Init(Text* text, uint16_t x, uint16_t y, uint8_t scale, uint8_t maxLength, const Font* font, uint16_t textColor, uint16_t backgroundColor)
{
	text->pos.x = x;
	text->pos.y = y;
	text->scale = scale;
	text->font = font;
	text->textColor = textColor;
	text->backgroundColor = backgroundColor;
	text->length = (maxLength > TEXT_MAX_CHARS) ? TEXT_MAX_CHARS : maxLength;
	text->text[0] = '\0';
	memset(text->currentlyVisible, ' ', sizeof text->currentlyVisible);
	text->valid = 0;
}

void Text_SetText(Text* text, const char* s)
{
	strncpy(text->text, s, text->length);
	text->text[text->length] = '\0';
}

void Text_Draw(Text* text)
{
	ILI9341_SetFont(text->font);
	int cellWidth = text->font->width * text->scale;
	int cellHeight = text->font->height * text->scale;
	int yOffset = 0, xOffset = 0;
	int ended = 0;
	for (int i = 0; i < text->length; i++)
	{
		if (text->text[i] == '\0') ended = 1;
		char want = ended ? ' ' : text->text[i];
		char currentChar = text->currentlyVisible[i];
		if (want == '\n' || text->pos.x + xOffset * cellWidth > DISPLAY_WIDTH - cellWidth)
		{
			yOffset++;
			xOffset = 0;
		}
		if (want != '\n')
		{
			if (text->valid == 0 || want != currentChar)
			{
				ILI9341_DrawCharScaled(text->pos.x + xOffset * cellWidth, text->pos.y + yOffset * cellHeight, (uint8_t)want, text->scale, text->textColor, text->backgroundColor);
				text->currentlyVisible[i] = want;
			}
			xOffset++;
		}
	}

	text->valid = 1;
}


void AttitudeDisplay_Init(AttitudeDisplay* display, Attitude* attitude, uint16_t x, uint16_t y, uint16_t radius)
{
	display->center.x = x;
	display->center.y = y;
	display->radius = radius;
	display->attitude = attitude;
}

static void FillSpan(int xa, int xb, int y, uint16_t color)
{
	if (xa <= xb) ILI9341_FillRectangle(xa, y, xb, y, color);
}

void AttitudeDisplay_Draw(AttitudeDisplay* display)
{
	float sr = sinf(display->attitude->roll  * DEG2RAD);
	float cr = cosf(display->attitude->roll  * DEG2RAD);
	float sp = sinf(display->attitude->pitch * DEG2RAD);
	float cp = cosf(display->attitude->pitch * DEG2RAD);

	int R  = display->radius;
	int cx = display->center.x;
	int cy = display->center.y;

	// A point on the horizon. The normal pointing toward the ground is (sr, cr).
	float hx = cx + R * sp * sr;
	float hy = cy + R * sp * cr;

	for (int dy = -R; dy <= R; dy++)
	{
		int y  = cy + dy;
		int w  = (int)sqrtf((float)(R * R - dy * dy));   // half-width of the circle on this row
		int xl = cx - w, xr = cx + w;

		float base = cr * (y - hy);

		if (fabsf(sr) < 1e-4f)                           // horizon (nearly) level: one color per row
		{
			FillSpan(xl, xr, y, (base > 0.0f) ? ILI9341_CHOCOLATE : ILI9341_SKYBLUE);
			continue;
		}

		float xc = hx - base / sr;                       // where this row crosses the horizon
		int s;                                           // last pixel of the left span
		if      (xc < xl - 1) s = xl - 1;
		else if (xc > xr)     s = xr;
		else                  s = (sr > 0.0f) ? (int)floorf(xc) : (int)ceilf(xc) - 1;
		if (s < xl - 1) s = xl - 1;

		FillSpan(xl,    s,  y, (sr < 0.0f) ? ILI9341_CHOCOLATE : ILI9341_SKYBLUE);
		FillSpan(s + 1, xr, y, (sr < 0.0f) ? ILI9341_SKYBLUE    : ILI9341_CHOCOLATE);
	}

	// Horizon line, ends exactly on the circle
	int L   = (int)lroundf(R * cp);
	int hxi = (int)lroundf(hx), hyi = (int)lroundf(hy);
	int ex  = (int)lroundf(L * cr), ey = (int)lroundf(L * sr);
	ILI9341_DrawLine(hxi - ex, hyi + ey, hxi + ex, hyi - ey, ILI9341_WHITE);

	ILI9341_DrawCircle(display->center, R, ILI9341_BLACK);

	// Fixed aircraft symbol (the spans painted over it, so redraw it every frame)
	ILI9341_FillRectangle(cx - 30, cy - 1, cx - 10, cy + 1, ILI9341_YELLOW);
	ILI9341_FillRectangle(cx + 10, cy - 1, cx + 30, cy + 1, ILI9341_YELLOW);
	ILI9341_FillRectangle(cx - 1,  cy - 1, cx + 1,  cy + 1, ILI9341_YELLOW);
}

void FlightUI_Init(FlightUI* ui, const Attitude* attitude, const char* title)
{
	ILI9341_FillScreen(ILI9341_WHITE);
	ILI9341_DrawStringScaled(40, 20, title, 1, 	ILI9341_BLACK, ILI9341_WHITE);
	ILI9341_DrawLine(40, 52, 260, 52, ILI9341_BLACK);

	Text_Init(&ui->accText, 8, 60, 1, 26, &Font_Spleen_8x16, ILI9341_BLACK, ILI9341_WHITE);
	Text_Init(&ui->gyroText, 8, 76, 1, 33, &Font_Spleen_8x16, ILI9341_BLACK, ILI9341_WHITE);
	Text_Init(&ui->tempText, 8, 92, 1, 15, &Font_Spleen_8x16, ILI9341_BLACK, ILI9341_WHITE);
	Text_Init(&ui->angleText, 7, 108, 1, 33, &Font_Spleen_8x16, ILI9341_BLACK, ILI9341_WHITE);
	AttitudeDisplay_Init(&ui->horizon, attitude, 160, 180, 40);
}

char x[12], y[12], z[12], line[TEXT_MAX_CHARS + 1];

static void Vec1Line(Text* box, const char* label, float v, uint8_t decimals, int width, const char* unit)
{
	FormatFixed(x, sizeof x, v, decimals);
	snprintf(line, sizeof line, "%s %*s %s", label, width, x, unit);
	Text_SetText(box, line);
	Text_Draw(box);
}

static void Vec2Line(Text* box, const char* label, float2 v, uint8_t decimals, int width, const char* unit)
{
	FormatFixed(x, sizeof x, v.x, decimals);
	FormatFixed(y, sizeof y, v.y, decimals);
	snprintf(line, sizeof line, "%s %*s %*s %s", label, width, x, width, y, unit);
	Text_SetText(box, line);
	Text_Draw(box);
}

static void Vec3Line(Text* box, const char* label, float3 v, uint8_t decimals, int width, const char* unit)
{
	FormatFixed(x, sizeof x, v.x, decimals);
	FormatFixed(y, sizeof y, v.y, decimals);
	FormatFixed(z, sizeof z, v.z, decimals);
	snprintf(line, sizeof line, "%s %*s %*s %*s %s", label, width, x, width, y, width, z, unit);
	Text_SetText(box, line);
	Text_Draw(box);
}

void FlightUI_Update(FlightUI* ui, float3 acc, float3 gyro, float temp)
{
	Vec3Line(&ui->accText, "ACC", acc, 2, 6, "g");
	Vec3Line(&ui->gyroText, "GYR", gyro, 1, 7, "deg/s");
	Vec1Line(&ui->tempText, "TEMP", temp, 1, 6, "C");
	float2 attitudeVals = {ui->horizon.attitude->roll, ui->horizon.attitude->pitch};
	Vec2Line(&ui->angleText, "ROLL+PITCH", attitudeVals, 1, 6, "deg");

	AttitudeDisplay_Draw(&ui->horizon);
}



