/*
 * SparkDisplayControl.h
 *
 *  Created on: 23.08.2021
 *      Author: stangreg
 */

#ifndef SPARKDISPLAYCONTROL_H_
#define SPARKDISPLAYCONTROL_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //https://github.com/adafruit/Adafruit_SSD1306

#include "SparkDataControl.h"

// OLED Screen config
#define SCREEN_WIDTH 128 // Display width
#define SCREEN_HEIGHT 64 // Display height
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define DISPLAY_MIN_X_FACTOR -12 // for text size 2, scales linearly with text size

// Splash logo measurements
#define imageWidth 128
#define imageHeight 47


class SparkDisplayControl {
public:
	SparkDisplayControl();
	SparkDisplayControl(SparkDataControl* dc);
	virtual ~SparkDisplayControl();
	void init(int mode);
	void update();


private:
	static Adafruit_SSD1306 display;
	SparkDataControl* spark_dc;
	//DISPLAY variables
	int display_x = 0;
	int display_minX = DISPLAY_MIN_X_FACTOR * 10;
	int display_scroll_num = -1; // scroll speed, make more negative to speed up the scroll

	SparkDataControl* dataControl() {return spark_dc;}

	// TODO This is the Sweaty X logo, should be exchanged to something different before publishing
	/*
	const unsigned char sweaty_logo_bmp [752] PROGMEM=
	{
			 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x87,0xff,0xe7
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x87,0xff,0xe3
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xff,0x23
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xfc,0xd1
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xfb,0xe1
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xf7,0xc7
			,0xfe,0x00,0x03,0xf0,0x3f,0x1f,0xff,0xc0,0xff,0xfc,0x3f,0xf1,0xff,0xe1,0xcf,0x8f
			,0xfc,0x00,0x03,0xf0,0x3f,0x1f,0xff,0xc0,0xff,0xfe,0x3f,0xe1,0xff,0xf9,0xfe,0x3f
			,0xf8,0x7f,0xe3,0xf0,0x3e,0x1f,0xff,0x80,0xff,0xfe,0x1f,0xc3,0xff,0xf3,0x7c,0x7f
			,0xf8,0xff,0xe1,0xf0,0x3e,0x1e,0x00,0x00,0x7f,0xff,0x0f,0xc7,0xff,0xfc,0xf9,0xff
			,0xf8,0xff,0xe1,0xe0,0x1e,0x1e,0x00,0x0c,0x7f,0xff,0x8f,0x87,0xff,0xf8,0xe3,0xff
			,0xf8,0xff,0xf1,0xe2,0x1e,0x3c,0x00,0x0c,0x3f,0xff,0x87,0x0f,0xff,0xf9,0xcf,0xff
			,0xf8,0x7f,0xf1,0xe3,0x1e,0x3f,0xff,0x0c,0x3f,0xff,0xc7,0x1f,0xff,0xf6,0x1f,0xff
			,0xf8,0x03,0xf0,0xe3,0x1e,0x3f,0xff,0x0e,0x3f,0xff,0xe2,0x1f,0xff,0xfe,0x3f,0xff
			,0xfc,0x00,0xf8,0xe3,0x0c,0x3f,0xff,0x1e,0x1f,0xff,0xe0,0x3f,0xff,0xfc,0x3f,0xff
			,0xff,0x00,0x78,0xc3,0x0c,0x3f,0xff,0x1e,0x1f,0xff,0xf0,0x7f,0xff,0xf9,0x1f,0xff
			,0xff,0xfc,0x78,0xc7,0x8c,0x78,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xf3,0x9f,0xff
			,0xff,0xfc,0x78,0x47,0x8c,0x78,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xf7,0x4f,0xff
			,0xff,0xfc,0x7c,0x47,0x8c,0x7f,0xfe,0x1f,0x87,0xc3,0xf8,0xff,0xff,0xdf,0x4f,0xff
			,0xff,0xfc,0x7c,0x07,0x80,0x7f,0xfe,0x3f,0x87,0xe3,0xf8,0xff,0xff,0xbf,0x27,0xff
			,0x80,0x00,0x7c,0x07,0xc0,0x7f,0xfc,0x3f,0xc7,0xe3,0xf8,0xff,0xff,0x7f,0xa7,0xff
			,0x00,0x00,0xfc,0x07,0xc0,0xff,0xfc,0x3f,0xc3,0xe3,0xf8,0xff,0xfe,0xff,0x97,0xff
			,0x00,0x01,0xfe,0x0f,0xc0,0x7f,0xfc,0x3f,0xe3,0xe3,0xf8,0xff,0xfd,0xff,0xdb,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xfb,0xff,0xe9,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xf7,0xff,0xe3,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xef,0xff,0xf3,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xdf,0xff,0xfb,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xbf,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0x7f,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xfe,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
			,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff

	};*/
	// 'Sparky_Logo', 128x47px
	const unsigned char epd_bitmap_Sparky_Logo [768] PROGMEM = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xfd, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xf3, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xcf, 0xef, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xef, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xf8, 0xef, 0x1f, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0x78, 0xff, 0xff, 0xf1, 0xec, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x1f, 0xff, 0xb7, 0xc0, 0xfb, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xb7, 0xce, 0x13, 0xa7, 0x81, 0xfb, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xfe, 0xff, 0xb7, 0xee, 0x63, 0xcf, 0xe5, 0xfb, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xf1, 0xff, 0x73, 0xcc, 0xe7, 0xcf, 0xe4, 0xfb, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xc7, 0xfe, 0xf3, 0xcd, 0xc3, 0xef, 0xe6, 0x7b, 0xcf, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0x9f, 0xfd, 0xf3, 0x1d, 0x93, 0xef, 0xe7, 0x3d, 0x8f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0x3f, 0xfb, 0xf0, 0x7c, 0x3b, 0xef, 0xe7, 0x8c, 0x2f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0x7f, 0xe7, 0xf3, 0xff, 0xfb, 0xef, 0xe7, 0xe3, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0x7f, 0xcf, 0xf3, 0xff, 0xfd, 0xff, 0xef, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0x7f, 0x0f, 0xfb, 0xff, 0xff, 0xff, 0xef, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0xfc, 0x1f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0xf0, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xfe, 0x41, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0x07, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x6f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xef, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xcf, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xcf, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x0f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 768)
	const int epd_bitmap_allArray_LEN = 1;
	const unsigned char* epd_bitmap_allArray[1] = {
		epd_bitmap_Sparky_Logo
	};

};

#endif /* SPARKDISPLAYCONTROL_H_ */
