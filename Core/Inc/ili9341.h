/*
 * ili9341.h
 *
 *  Created on: 10/10/2026
 *      Author: samuelrose
 */

#ifndef SRC_ILI9341_H_
#define SRC_ILI9341_H_

#include <stdint.h>
#include "main.h"
#include "font.h"
#include "linalg.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define FILL_BUFFER_SIZE 512

#define ILI9341_ALICEBLUE 0xF7DF
#define ILI9341_ANTIQUEWHITE 0xFF5A
#define ILI9341_AQUA 0x07FF
#define ILI9341_AQUAMARINE 0x7FFA
#define ILI9341_AZURE 0xF7FF
#define ILI9341_BEIGE 0xF7BB
#define ILI9341_BISQUE 0xFF38
#define ILI9341_BLACK 0x0000 /*   0, 0, 0 */
#define ILI9341_BLANCHEDALMOND 0xFF59
#define ILI9341_BLUE 0x001F /*   0, 0, 255 */
#define ILI9341_BLUEVIOLET 0x895C
#define ILI9341_BROWN 0xA145
#define ILI9341_BURLYWOOD 0xDDD0
#define ILI9341_CADETBLUE 0x5CF4
#define ILI9341_CHARTREUSE 0x7FE0
#define ILI9341_CHOCOLATE 0xD343
#define ILI9341_CORAL 0xFBEA
#define ILI9341_CORNFLOWERBLUE 0x64BD
#define ILI9341_CORNSILK 0xFFDB
#define ILI9341_CRIMSON 0xD8A7
#define ILI9341_CYAN 0x07FF /*   0, 255, 255 */
#define ILI9341_DARKBLUE 0x0011
#define ILI9341_DARKCYAN 0x03EF /*   0, 128, 128 */
#define ILI9341_DARKCYAN2 0x0451
#define ILI9341_DARKGOLDENROD 0xBC21
#define ILI9341_DARKGRAY 0xAD55
#define ILI9341_DARKGREEN2 0x0320
#define ILI9341_DARKGREEN 0x03E0 /*   0, 128, 0 */
#define ILI9341_DARKGREY 0x7BEF /* 128, 128, 128 */
#define ILI9341_DARKKHAKI 0xBDAD
#define ILI9341_DARKMAGENTA 0x8811
#define ILI9341_DARKOLIVEGREEN 0x5345
#define ILI9341_DARKORANGE 0xFC60
#define ILI9341_DARKORCHID 0x9999
#define ILI9341_DARKRED 0x8800
#define ILI9341_DARKSALMON 0xECAF
#define ILI9341_DARKSEAGREEN 0x8DF1
#define ILI9341_DARKSLATEBLUE 0x49F1
#define ILI9341_DARKSLATEGRAY 0x2A69
#define ILI9341_DARKTURQUOISE 0x067A
#define ILI9341_DARKVIOLET 0x901A
#define ILI9341_DEEPPINK 0xF8B2
#define ILI9341_DEEPSKYBLUE 0x05FF
#define ILI9341_DIMGRAY 0x6B4D
#define ILI9341_DODGERBLUE 0x1C9F
#define ILI9341_FIREBRICK 0xB104
#define ILI9341_FLORALWHITE 0xFFDE
#define ILI9341_FORESTGREEN 0x2444
#define ILI9341_FUCHSIA 0xF81F
#define ILI9341_GAINSBORO 0xDEFB
#define ILI9341_GHOSTWHITE 0xFFDF
#define ILI9341_GOLD 0xFEA0
#define ILI9341_GOLDENROD 0xDD24
#define ILI9341_GRAY 0x8410
#define ILI9341_GREEN2 0x0400
#define ILI9341_GREEN 0x07E0 /*   0, 255, 0 */
#define ILI9341_GREENYELLOW 0xAFE5 /* 173, 255, 47 */
#define ILI9341_HONEYDEW 0xF7FE
#define ILI9341_HOTPINK 0xFB56
#define ILI9341_INDIANRED 0xCAEB
#define ILI9341_INDIGO 0x4810
#define ILI9341_IVORY 0xFFFE
#define ILI9341_KHAKI 0xF731
#define ILI9341_LAVENDER 0xE73F
#define ILI9341_LAVENDERBLUSH 0xFF9E
#define ILI9341_LAWNGREEN 0x7FE0
#define ILI9341_LEMONCHIFFON 0xFFD9
#define ILI9341_LIGHTBLUE 0xAEDC
#define ILI9341_LIGHTCORAL 0xF410
#define ILI9341_LIGHTCYAN 0xE7FF
#define ILI9341_LIGHTGOLDENRODYELLOW 0xFFDA
#define ILI9341_LIGHTGREEN 0x9772
#define ILI9341_LIGHTGREY 0xC618 /* 192, 192, 192 */
#define ILI9341_LIGHTGREY2 0xD69A
#define ILI9341_LIGHTPINK 0xFDB8
#define ILI9341_LIGHTSALMON 0xFD0F
#define ILI9341_LIGHTSEAGREEN 0x2595
#define ILI9341_LIGHTSKYBLUE 0x867F
#define ILI9341_LIGHTSLATEGRAY 0x7453
#define ILI9341_LIGHTSTEELBLUE 0xB63B
#define ILI9341_LIGHTYELLOW 0xFFFC
#define ILI9341_LIME 0x07E0
#define ILI9341_LIMEGREEN 0x3666
#define ILI9341_LINEN 0xFF9C
#define ILI9341_MAGENTA 0xF81F /* 255, 0, 255 */
#define ILI9341_MAROON 0x7800 /* 128, 0, 0 */
#define ILI9341_MAROON2 0x8000
#define ILI9341_MEDIUMAQUAMARINE 0x6675
#define ILI9341_MEDIUMBLUE 0x0019
#define ILI9341_MEDIUMORCHID 0xBABA
#define ILI9341_MEDIUMPURPLE 0x939B
#define ILI9341_MEDIUMSEAGREEN 0x3D8E
#define ILI9341_MEDIUMSLATEBLUE 0x7B5D
#define ILI9341_MEDIUMSPRINGGREEN 0x07D3
#define ILI9341_MEDIUMTURQUOISE 0x4E99
#define ILI9341_MEDIUMVIOLETRED 0xC0B0
#define ILI9341_MIDNIGHTBLUE 0x18CE
#define ILI9341_MINTCREAM 0xF7FF
#define ILI9341_MISTYROSE 0xFF3C
#define ILI9341_MOCCASIN 0xFF36
#define ILI9341_NAVAJOWHITE 0xFEF5
#define ILI9341_NAVY 0x000F /*   0, 0, 128 */
#define ILI9341_NAVY2 0x0010
#define ILI9341_OLDLACE 0xFFBC
#define ILI9341_OLIVE 0x7BE0 /* 128, 128, 0 */
#define ILI9341_OLIVE2 0x8400
#define ILI9341_OLIVEDRAB 0x6C64
#define ILI9341_ORANGE 0xFD20 /* 255, 165, 0 */
#define ILI9341_ORANGERED 0xFA20
#define ILI9341_ORCHID 0xDB9A
#define ILI9341_PALEGOLDENROD 0xEF55
#define ILI9341_PALEGREEN 0x9FD3
#define ILI9341_PALETURQUOISE 0xAF7D
#define ILI9341_PALEVIOLETRED 0xDB92
#define ILI9341_PAPAYAWHIP 0xFF7A
#define ILI9341_PEACHPUFF 0xFED7
#define ILI9341_PERU 0xCC27
#define ILI9341_PINK 0xF81F
#define ILI9341_PINK_2 0xFE19
#define ILI9341_PLUM 0xDD1B
#define ILI9341_POWDERBLUE 0xB71C
#define ILI9341_PURPLE 0x780F /* 128, 0, 128 */
#define ILI9341_PURPLE2 0x8010
#define ILI9341_RED 0xF800 /* 255, 0, 0 */
#define ILI9341_ROSYBROWN 0xBC71
#define ILI9341_ROYALBLUE 0x435C
#define ILI9341_SADDLEBROWN 0x8A22
#define ILI9341_SALMON 0xFC0E
#define ILI9341_SANDYBROWN 0xF52C
#define ILI9341_SEAGREEN 0x2C4A
#define ILI9341_SEASHELL 0xFFBD
#define ILI9341_SIENNA 0xA285
#define ILI9341_SILVER 0xC618
#define ILI9341_SKYBLUE 0x867D
#define ILI9341_SLATEBLUE 0x6AD9
#define ILI9341_SLATEGRAY 0x7412
#define ILI9341_SNOW 0xFFDF
#define ILI9341_SPRINGGREEN 0x07EF
#define ILI9341_STEELBLUE 0x4416
#define ILI9341_TAN 0xD5B1
#define ILI9341_TEAL 0x0410
#define ILI9341_THISTLE 0xDDFB
#define ILI9341_TOMATO 0xFB08
#define ILI9341_TURQUOISE 0x471A
#define ILI9341_VIOLET 0xEC1D
#define ILI9341_WHEAT 0xF6F6
#define ILI9341_WHITE 0xFFFF /* 255, 255, 255 */
#define ILI9341_WHITESMOKE 0xF7BE
#define ILI9341_YELLOW 0xFFE0 /* 255, 255, 0 */
#define ILI9341_YELLOWGREEN 0x9E66

// Level 1 Commands
#define ILI9341_NOP 					0x00 // No-operation
#define ILI9341_SWRESET 				0x01 // Software reset
#define ILI9341_RDDIDIF 				0x04 // Read display identification information
#define ILI9341_RDDST 					0x09 // Read display status
#define ILI9341_RDDPM 					0x0A // Read display power mode
#define ILI9341_RDDMADCTL				0x0B // Read display MADCTL
#define ILI9341_RDDCOLMOD				0x0C // Read display pixel format
#define ILI9341_RDDIM					0x0D // Read display image mode
#define ILI9341_RDDSM 					0x0E // Read display signal mode
#define ILI9341_RDDSDR					0x0F // Read display self-diagnostic result
#define ILI9341_SLPIN					0x10 // Enter sleep mode
#define ILI9341_SLPOUT					0x11 // Sleep out
#define ILI9341_PLTON					0x12 // Partial mode ON
#define ILI9341_NORON					0x13 // Normal display mode ON
#define ILI9341_DINVOFF					0x20 // Display inversion OFF
#define ILI9341_DINVON					0x21 // Display inversion ON
#define ILI9341_GAMSET					0x26 // Gamma set
#define ILI9341_DISPOFF					0x28 // Display OFF
#define ILI9341_DISPON					0x29 // Display ON
#define ILI9341_CASET					0x2A // Column address set
#define ILI9341_PASET					0x2B // Page address set
#define ILI9341_RAMWR					0x2C // Memory write
#define ILI9341_RGBSET					0x2D // Define LUT for 16->18 bit color conversion
#define ILI9341_RAMRD					0x2E // Memory read
#define ILI9341_PLTAR					0x30 // Define partial mode's display area
#define ILI9341_VSCRDEF					0x33 // Vertical scrolling definition
#define ILI9341_TEOFF					0x34 // Tearing effect line OFF
#define ILI9341_TEON					0x35 // Tearing effect line ON
#define ILI9341_MADCTL					0x36 // Memory access control
#define ILI9341_VSCRSADD				0x37 // Vertical scrolling start address
#define ILI9341_IDMOFF					0x38 // Idle mode OFF
#define ILI9341_IDMON					0x39 // Idle mode ON
#define ILI9341_COLMOD					0x3A // Pixel format set
#define ILI9341_Write_Memory_Continue	0x3C // Write image data continuing from last write
#define ILI9341_Read_Memory_Continue	0x3E // Read image data continuing from last read
#define ILI9341_Set_Tear_Scanline		0x44 // Tearing effect when display reaches line STS
#define ILI9341_Get_Scanline			0x45 // Returns current scanline
#define ILI9341_WRDISBV					0x51 // Write display brightness
#define ILI9341_RDDISBV					0x52 // Read display brightness
#define ILI9341_WRCTRLD					0x53 // Write control display (used for brightness)
#define ILI9341_RDCTRLD					0x54 // Read control display
#define ILI9341_WRCABC					0x55 // Write content adaptive brightness control
#define ILI9341_RDCABC					0x56 // Read content adaptive brightness control
#define ILI9341_WRCABCMIN				0x5E // Write CABC minimum brightness
#define ILI9341_RDCABCMIN				0x5F // Read CABC minimum brightness
#define ILI9341_RDID1					0xDA // Read ID1 (identifies LCD module manufacturer ID)
#define ILI9341_RDID2					0xDB // Read ID2 (LCD module/driver version)
#define ILI9341_RDID3					0xDC // Read ID3 (LCD module/driver; user-specified)

// Level 2 Commands
#define ILI9341_IFMODE					0xB0 // Interface mode control
#define ILI9341_FRMCTR1					0xB1 // Frame rate control (in normal mode / full colors)
#define ILI9341_FRMCTR2					0xB2 // Frame rate control (in idle mode / 8 colors)
#define ILI9341_FRMCTR3					0xB3 // Frame rate control (in partial mode / full colors)
#define ILI9341_INVTR					0xB4 // Display inversion control
#define ILI9341_PRCTR					0xB5 // Blanking porch
#define ILI9341_DISCTRL					0xB6 // Display function control
#define ILI9341_ETMOD					0xB7 // Entry mode set
#define ILI9341_BLCTRL1					0xB8 // Backlight control 1
#define ILI9341_BLCTRL2					0xB9 // Backlight control 2
#define ILI9341_BLCTRL3					0xBA // Backlight control 3
#define ILI9341_BLCTRL4					0xBB // Backlight control 4
#define ILI9341_BLCTRL5					0xBC // Backlight control 5
#define ILI9341_BLCTRL7					0xBE // Backlight control 7
#define ILI9341_BLCTRL8					0xBF // Backlight control 8
#define ILI9341_PWCTRL1					0xC0 // Power control 1
#define ILI9341_PWCTRL2					0xC1 // Power control 2
#define ILI9341_VMCTRL1					0xC5 // VCOM control 1
#define ILI9341_VMCTRL2					0xC7 // VCOM control 2
#define ILI9341_NVMWR					0xD0 // NV memory write
#define ILI9341_NVMPKEY					0xD1 // NV memory protection key
#define ILI9341_RDNVM					0xD2 // NV memory status read
#define ILI9341_RDID4					0xD3 // Read ID4 (IC device code)
#define ILI9341_PGAMCTRL				0xE0 // Positive gamma control
#define ILI9341_NGAMCTRL				0xE1 // Negative gamma control
#define ILI9341_DGAMCTRL1				0xE2 // Digital gamma control 1
#define ILI9341_DGAMCTRL2				0xE3 // Digital gamma control 2
#define ILI9341_IFCTL					0xF6 // 16 bits data format selection

uint16_t R8G8B8_to_R5G6B5(uint8_t r, uint8_t g, uint8_t b);

void ILI9341_Init(SPI_HandleTypeDef* hspi);

void ILI9341_FillRectangle(int x0, int y0, int x1, int y1, uint16_t color);

void ILI9341_FillScreen(uint16_t color);

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void ILI9341_DrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void ILI9341_DrawShadedRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t outlineColor, uint16_t fillColor);

void ILI9341_DrawTri(ushort2 a, ushort2 b, ushort2 c, uint16_t color);

void ILI9341_DrawShadedTri(ushort2 a, ushort2 b, ushort2 c, uint16_t outlineColor, uint16_t fillColor);

void ILI9341_DrawCircle(ushort2 c, uint16_t r, uint16_t color);

void ILI9341_DrawShadedCircle(ushort2 c, uint16_t r, uint16_t outlineColor, uint16_t fillColor);

void ILI9341_SetFont(const Font* font);

void ILI9341_DrawChar(uint16_t x, uint16_t y, uint8_t glyph, uint16_t textColor, uint16_t backgroundColor);

void ILI9341_DrawCharScaled(uint16_t x, uint16_t y, uint8_t glyph, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);

void ILI9341_DrawString(uint16_t x, uint16_t y, const char* text, uint16_t textColor, uint16_t backgroundColor);

void ILI9341_DrawStringScaled(uint16_t x, uint16_t y, const char* text, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);

#endif /* SRC_ILI9341_H_ */
