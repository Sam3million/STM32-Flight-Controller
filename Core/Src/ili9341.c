/*
 * ili9341.c
 *
 *  Created on: 10/10/2026
 *      Author: samuelrose
 */

#include "ili9341.h"
#include "util.h"
#include <stdlib.h>
#include <stddef.h>

static SPI_HandleTypeDef* displaySPI;
static uint8_t fillBuffer[FILL_BUFFER_SIZE];
static Font* currentFont;

static void SendCommand(uint8_t code)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 0);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(displaySPI, &code, 1, 100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}

static void SendData(uint8_t* buffer, uint16_t length)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(displaySPI, buffer, length, 100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}

static void SendByte(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, 1);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);
	HAL_SPI_Transmit(displaySPI, &data, 1, 100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
}

void ILI9341_Init(SPI_HandleTypeDef* hspi)
{
	displaySPI = hspi;

	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 0);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 1);
	HAL_Delay(120);

	SendCommand(ILI9341_SWRESET);
	HAL_Delay(120);

	SendCommand(ILI9341_SLPOUT);
	HAL_Delay(120);

	SendCommand(ILI9341_COLMOD);
	// 16 bit color format, DBI interface:
	uint8_t colorFormat = 0x05;
	SendData(&colorFormat, 1);

	SendCommand(ILI9341_MADCTL);
	uint8_t madctlFormat = 0b11101000;
	SendData(&madctlFormat, 1);

	SendCommand(ILI9341_DISPON);
}

static void SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	SendCommand(ILI9341_CASET);
	uint8_t message[4];
	message[0] = (uint8_t)(x0 >> 8);
	message[1] = (uint8_t)(x0 & 0xFF);
	message[2] = (uint8_t)(x1 >> 8);
	message[3] = (uint8_t)(x1 & 0xFF);
	SendData(&(message[0]), 4);
	SendCommand(ILI9341_PASET);
	message[0] = (uint8_t)(y0 >> 8);
	message[1] = (uint8_t)(y0 & 0xFF);
	message[2] = (uint8_t)(y1 >> 8);
	message[3] = (uint8_t)(y1 & 0xFF);
	SendData(&(message[0]), 4);
	SendCommand(ILI9341_RAMWR);
}

void ILI9341_FillRectangle(int x0, int y0, int x1, int y1, uint16_t color)
{
	if (x0 > x1) SwapInt(&x0, &x1);
	if (y0 > y1) SwapInt(&y0, &y1);
	if (x1 < 0 || y1 < 0 || x0 >= DISPLAY_WIDTH || y0 >= DISPLAY_HEIGHT) return;
	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= DISPLAY_WIDTH)  x1 = DISPLAY_WIDTH  - 1;
	if (y1 >= DISPLAY_HEIGHT) y1 = DISPLAY_HEIGHT - 1;

	uint32_t remaining = (uint32_t)(x1 - x0 + 1) * (y1 - y0 + 1) * 2;

	uint32_t prefill = (remaining < FILL_BUFFER_SIZE) ? remaining : FILL_BUFFER_SIZE;
	for (uint32_t i = 0; i < prefill; i += 2)
	{
		fillBuffer[i]     = (uint8_t)(color >> 8);
		fillBuffer[i + 1] = (uint8_t)(color & 0xFF);
	}

	SetAddressWindow(x0, y0, x1, y1);

	while (remaining > 0)
	{
		uint16_t chunk = (remaining < FILL_BUFFER_SIZE) ? (uint16_t)remaining : FILL_BUFFER_SIZE;
		SendData(fillBuffer, chunk);
		remaining -= chunk;
	}
}

void ILI9341_FillScreen(uint16_t color)
{
	ILI9341_FillRectangle(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, color);
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	SetAddressWindow(x, y, x, y);
	SendByte(color >> 8);
	SendByte(color);
}

void ILI9341_DrawLine(uint16_t ux0, uint16_t uy0, uint16_t ux1, uint16_t uy1, uint16_t color)
{
	// Perfectly vertical or horizontal lines and single pixels can be drawn with a rectangle
	if (ux0 == ux1 || uy0 == uy1)
	{
		uint16_t xa = (ux0 < ux1) ? ux0 : ux1;
		uint16_t xb = (ux0 < ux1) ? ux1 : ux0;
		uint16_t ya = (uy0 < uy1) ? uy0 : uy1;
		uint16_t yb = (uy0 < uy1) ? uy1 : uy0;
		ILI9341_FillRectangle(xa, ya, xb, yb, color);
		return;
	}

	int x0 = ux0, y0 = uy0, x1 = ux1, y1 = uy1;

	int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		SwapInt(&x0, &y0);
		SwapInt(&x1, &y1);
	}

	if (x0 > x1)
	{
		SwapInt(&x0, &x1);
		SwapInt(&y0, &y1);
	}

	int dx = x1 - x0;
	int dy = abs(y1 - y0);
	int ystep = (y0 < y1) ? 1 : -1;

	int D = 2 * dy - dx;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (steep) ILI9341_DrawPixel(y, x, color);
		else       ILI9341_DrawPixel(x, y, color);

		if (D > 0)
		{
			y += ystep;
			D -= 2 * dx;
		}
		D += 2 * dy;
	}
}

void ILI9341_DrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	ILI9341_DrawLine(x0, y0, x0, y1, color);
	ILI9341_DrawLine(x0, y0, x1, y0, color);
	ILI9341_DrawLine(x0, y1, x1, y1, color);
	ILI9341_DrawLine(x1, y0, x1, y1, color);
}

void ILI9341_DrawShadedRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t outlineColor, uint16_t fillColor)
{
	if (x0 > x1) SwapUInt16(&x0, &x1);
	if (y0 > y1) SwapUInt16(&y0, &y1);

	if (x1 - x0 >= 2 && y1 - y0 >= 2)
	{
		ILI9341_FillRectangle(x0 + 1, y0 + 1, x1 - 1, y1 - 1, fillColor);
	}

	ILI9341_DrawRect(x0, y0, x1, y1, outlineColor);
}

void ILI9341_DrawTri(ushort2 a, ushort2 b, ushort2 c, uint16_t color)
{
	ILI9341_DrawLine(a.x, a.y, b.x, b.y, color);
	ILI9341_DrawLine(a.x, a.y, c.x, c.y, color);
	ILI9341_DrawLine(b.x, b.y, c.x, c.y, color);
}

static int DivRound(int num, int den)   // den > 0; rounds half away from zero
{
	return (num >= 0) ? (num + den / 2) / den : -((-num + den / 2) / den);
}

// x of edge (x0,y0)-(x1,y1) on row y. Requires y1 > y0.
static int EdgeX(int x0, int y0, int x1, int y1, int y)
{
	return x0 + DivRound((x1 - x0) * (y - y0), y1 - y0);
}

void ILI9341_DrawShadedTri(ushort2 a, ushort2 b, ushort2 c, uint16_t outlineColor, uint16_t fillColor)
{
	int ax = a.x, ay = a.y, bx = b.x, by = b.y, cx = c.x, cy = c.y;

	// Sort by y
	if (ay > by) { SwapInt(&ax, &bx); SwapInt(&ay, &by); }
	if (by > cy) { SwapInt(&bx, &cx); SwapInt(&by, &cy); }
	if (ay > by) { SwapInt(&ax, &bx); SwapInt(&ay, &by); }

	if (ay == cy)
	{
		// All three points on one row: just a horizontal line
		int lo = MinInt(ax, MinInt(bx, cx));
		int hi = MaxInt(ax, MaxInt(bx, cx));
		ILI9341_FillRectangle(lo, ay, hi, ay, fillColor);
	}
	else
	{
		int yStart = MaxInt(ay, 0);
		int yEnd   = MinInt(cy, DISPLAY_HEIGHT - 1);

		for (int y = yStart; y <= yEnd; y++)
		{
			int xLong = EdgeX(ax, ay, cx, cy, y);      // long edge a->c, crosses every row

			int xShort;
			if (y < by)         xShort = EdgeX(ax, ay, bx, by, y);   // upper half: edge a->b
			else if (cy == by)  xShort = bx;                          // flat bottom: b->c is horizontal
			else                xShort = EdgeX(bx, by, cx, cy, y);   // lower half: edge b->c

			ILI9341_FillRectangle(xLong, y, xShort, y, fillColor);   // sorts the two ends itself
		}
	}

	ILI9341_DrawTri(a, b, c, outlineColor);
}

void ILI9341_DrawCircle(ushort2 c, uint16_t r, uint16_t color)
{
	int t1 = r >> 4;
	int t2;
	int x = r;
	int y = 0;
	while (x >= y)
	{
		ILI9341_DrawPixel(c.x + x, c.y + y, color);
		ILI9341_DrawPixel(c.x + x, c.y - y, color);
		ILI9341_DrawPixel(c.x - x, c.y + y, color);
		ILI9341_DrawPixel(c.x - x, c.y - y, color);
		ILI9341_DrawPixel(c.x + y, c.y + x, color);
		ILI9341_DrawPixel(c.x + y, c.y - x, color);
		ILI9341_DrawPixel(c.x - y, c.y + x, color);
		ILI9341_DrawPixel(c.x - y, c.y - x, color);

		y++;
		t1 += y;
		t2 = t1 - x;

		if (t2 >= 0)
		{
			t1 = t2;
			x--;
		}
	}
}

static void CircleCaps(ushort2 c, int capX, int capW, uint16_t color)
{
	ILI9341_FillRectangle(c.x - capW, c.y + capX, c.x + capW, c.y + capX, color);
	ILI9341_FillRectangle(c.x - capW, c.y - capX, c.x + capW, c.y - capX, color);
}

void ILI9341_DrawShadedCircle(ushort2 c, uint16_t r, uint16_t outlineColor, uint16_t fillColor)
{
	int t1 = r >> 4;
	int t2;
	int x = r;
	int y = 0;
	int capX = 0, capW = 0, capPending = 0;

	while (x >= y)
	{
		ILI9341_FillRectangle(c.x - x, c.y + y, c.x + x, c.y + y, fillColor);
		if (y != 0) ILI9341_FillRectangle(c.x - x, c.y - y, c.x + x, c.y - y, fillColor);

		capX = x;
		capW = y;
		capPending = 1;

		y++;
		t1 += y;
		t2 = t1 - x;
		if (t2 >= 0)
		{
			t1 = t2;
			x--;
			CircleCaps(c, capX, capW, fillColor);
			capPending = 0;
		}
	}
	if (capPending) CircleCaps(c, capX, capW, fillColor);

	// Outline on top
	ILI9341_DrawCircle(c, r, outlineColor);
}

void ILI9341_SetFont(const Font* font)
{
	currentFont = font;
}

void ILI9341_DrawChar(uint16_t x, uint16_t y, uint8_t ch, uint16_t textColor, uint16_t backgroundColor)
{
	ILI9341_DrawCharScaled(x, y, ch, 1, textColor, backgroundColor);
}

void ILI9341_DrawCharScaled(uint16_t x, uint16_t y, uint8_t ch, uint8_t scale, uint16_t textColor, uint16_t backgroundColor)
{
	const Font *f = currentFont;
	if (f == NULL) return;
	if (ch < f->firstChar || ch >= f->firstChar + f->numChars) return;
	if (scale == 0 || f->width * scale * 2 > FILL_BUFFER_SIZE) return;
	if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;

	int visibleW = MinInt(f->width  * scale, DISPLAY_WIDTH  - x);
	int visibleH = MinInt(f->height * scale, DISPLAY_HEIGHT - y);

	int bytesPerRow = (f->width + 7) / 8;
	const uint8_t *glyph = f->data + (ch - f->firstChar) * bytesPerRow * f->height;

	SetAddressWindow(x, y, x + visibleW - 1, y + visibleH - 1);

	int rowsSent = 0;
	for (int row = 0; row < f->height; row++)
	{
		const uint8_t *rowData = glyph + row * bytesPerRow;

		// Build scaled row, clipped to the visible width
		int idx = 0;
		for (int col = 0; col < f->width && idx < visibleW * 2; col++)
		{
			uint8_t bits = rowData[col >> 3];
			int shift = f->msbFirst ? (7 - (col & 7)) : (col & 7);
			uint16_t color = ((bits >> shift) & 1) ? textColor : backgroundColor;

			for (int rep = 0; rep < scale && idx < visibleW * 2; rep++)
			{
				fillBuffer[idx++] = color >> 8;
				fillBuffer[idx++] = color & 0xFF;
			}
		}

		// Send that row multiple times for vertical scaling
		for (int rep = 0; rep < scale; rep++)
		{
			SendData(fillBuffer, visibleW * 2);
			if (++rowsSent == visibleH) return;
		}
	}
}


void ILI9341_DrawString(uint16_t x, uint16_t y, const char* text, uint16_t textColor, uint16_t backgroundColor)
{
	ILI9341_DrawStringScaled(x, y, text, 1, textColor, backgroundColor);
}

void ILI9341_DrawStringScaled(uint16_t x, uint16_t y, const char* text, uint8_t scale, uint16_t textColor, uint16_t backgroundColor)
{
	if (!currentFont) return;
	int i = 0;
	while(text[i] != '\0')
	{
		if (x + i * 8 * scale >= DISPLAY_WIDTH) break;
		ILI9341_DrawCharScaled(x + i * currentFont->width * scale, y, text[i], scale, textColor, backgroundColor);
		i++;
	}
}





