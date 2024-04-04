/* 
* ws2812.h
*
* Created: 11/27/2022 10:21:59 PM
* Author: Tower
*/

#ifndef __WS2812_H__
#define __WS2812_H__
#include <stdlib.h>
#define __DELAY_BACKWARD_COMPATIBLE__
//#include <util/delay.h>
//#include <avr/pgmspace.h>
#include "UtilFuncs.h"
#include "light_ws2812.h"
#include "hardware.h"
#include "CharTable.h"
#include <string.h> //used for memcpy

#include <stdint.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <thread>

#define LEFTEYE 0
#define RIGHTEYE 1
#define BOTHEYE 2

#define RAND_AREA 0
#define RAND_PERIMETER 1
#define BLACK 0x000000
#define M_PI 3.141592653589793
extern uint8_t randSelect;
extern std::vector<sf::RectangleShape> Pixels;
extern sf::RenderWindow* window;

class ws2812 {
	
public:
	uint8_t green = 0;
	uint8_t red = 0;
	uint8_t blue = 0;
	uint32_t Forecolor = 0;
	uint32_t wheelAngle = 0; //used/modified in many patterns to spin the color wheel vector
	
	ws2812(uint16_t numLeds);
	~ws2812();
	
	void setPixelColor(int, uint32_t);
	uint32_t getPixelColor(int);
	void show();
	uint32_t Color(uint8_t, uint8_t, uint8_t);
	void setBrightness(uint8_t);
	uint8_t getBrightness();
	
	void FillScreen(uint32_t c);
	void FillColumn(int n, uint32_t c);
	
	void printStringWithShiftL(const char* s, int shift_speed);
	void printCharWithShiftL(char c, int shift_speed);
	void ShowBitmap(const uint8_t* bitmap, uint32_t c, uint8_t eyeSelect=2);
	
	void HorizShiftL(int count, uint8_t withShow);
	void HorizShiftR(int count, uint8_t withShow);
	
	void scaleScreen(float sclPercent, uint8_t originX, uint8_t originY);
	void rotateScreen(float angPercent, int dir, int originX, int originY);
	//void recflectScreenXY(int xR, int yR);
	
	int getGreen(uint32_t c);//these could be 1 function with an extra parameter (RED, GREEN, BLUE which define bit shifting)
	int getRed(uint32_t c);
	int getBlue(uint32_t c);
	

	//wheelColor(int angle , magnitude) //angle is 0 to 360, magnitude is 0 to 3*panel.getbrtightness();
	//wheelColor(RANDAREA)
	//wheelColor(RANDPERIMETER)
	
	//0 = A color is mapped to anypoint in the color wheel (included Dim and Bright Colors). Range is based on _brightness
	//1 = A color is mapped to the perimeter of the color wheel (only bright Colors).Range is based on _brightness
	uint32_t wheelColor(int select);
	uint32_t wheelColor(int angle, int magnitude);

	void updateColorVars(uint32_t c); //updates Forecolor, green, red, blue	
	
	//uint32_t getrand(uint8_t _randSelect); //can return "dim" colors. replaced
	uint32_t Wheel(uint32_t WheelPos, uint32_t max); //wont return "dim" colors
	
private:
	uint16_t _numLeds;
	struct cRGB* _dispBuffer; //color data stored in struct array order is really GRB. dumb naming in the lib
	uint8_t _brightness;
};

#endif //__WS2812_H__