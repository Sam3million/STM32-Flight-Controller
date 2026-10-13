/*
 * ui_elements.c
 *
 *  Created on: 13/10/2026
 *      Author: samuelrose
 */


#include "ui_elements.h"
#include "ili9341.h"
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
		if (currentChar == '\n' || text->pos.x + xOffset * cellWidth > DISPLAY_WIDTH - cellWidth)
		{
			yOffset++;
			xOffset = 0;
		}
		if (currentChar != '\n')
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
