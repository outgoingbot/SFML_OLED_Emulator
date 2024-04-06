/*
SSD1306 OLED Emulator
*/

#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <thread>
#include <Windows.h>    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include "ssd1306.h"
#include "bitmap.h"

//SFML Window Size
#define WINDOW_WIDTH (1920-750)
#define WINDOW_HEIGHT (1080-400)

//adjust where the "OLED' is placed reletive to the window Origin
#define PADDING_WINDOW_X 70
#define PADDING_WINDOW_Y 50

//OLED PARAMS
#define NUMCOLS 128
#define NUMROWS 32
#define NUM_PAGES 4
#define NUM_PIXELS (NUMCOLS*NUMROWS) //used rectange vector
#define ROWS_PER_PAGE (NUMROWS/NUM_PAGES)
#define PIXELS_PER_PAGE (ROWS_PER_PAGE * NUMCOLS)
#define PIXEL_SIZE 5
#define DELTA_XY 8 //Spacing of the Pixels (larger is more spread out)
#define PAGE_HEIGHT (DELTA_XY * ROWS_PER_PAGE) 


//-----------------------------------Global Variables------------------------------------

//Graphics
sf::RenderWindow* window = nullptr;

sf::CircleShape* TBpad = new sf::CircleShape(100);
sf::CircleShape* TB = new sf::CircleShape(50);
std::vector<sf::RectangleShape> LED_RGBW;

//User Input
sf::Vector2f mousePosf;

//Make Pixels array
std::vector<sf::RectangleShape> Pixels;
sf::Color outlineColor = sf::Color(50, 50, 50);
sf::Color onColor = sf::Color(66, 245, 212);
sf::Color offColor = sf::Color::Black;

#ifdef __cplusplus
extern "C" {
	uint8_t PixelsDispBuffer[512]; //must match the SSD1306 Buffer size
}
#endif





//--------------------------------------User Function Definitions--------------------------------------
bool isMouseOverRect(sf::Vector2f* mousePosition, sf::RectangleShape* RS) {
	if (mousePosition->x > RS->getPosition().x && mousePosition->x < RS->getPosition().x + RS->getSize().x) {
		if (mousePosition->y > RS->getPosition().y && mousePosition->y < RS->getPosition().y + RS->getSize().y) {
			return true;
		}
	}
	return false;
}

int mapXYtoRect(uint32_t x, uint32_t y) {
	if (x >= NUMCOLS) return 0;
	if (x < 0) return 0;

	if (y >= NUMROWS) return 0;
	if (y < 0) return 0;

	uint32_t p = 0;
	//map 2d coordinates to rectange array Pixels
	if (y < ROWS_PER_PAGE * 4) p = 3;
	if (y < ROWS_PER_PAGE * 3) p = 2;
	if (y < ROWS_PER_PAGE * 2) p = 1;
	if (y < ROWS_PER_PAGE * 1) p = 0;

	//not sure why I need to shift the x axis back by p.
	return ((p * PIXELS_PER_PAGE) + ((x - p) * ROWS_PER_PAGE) + y);
}

sf::Vector2i mapRecttoXY(uint32_t i) {
	uint32_t p, x, y;
	if (i < PIXELS_PER_PAGE * 4) p = 3;
	if (i < PIXELS_PER_PAGE * 3) p = 2;
	if (i < PIXELS_PER_PAGE * 2) p = 1;
	if (i < PIXELS_PER_PAGE * 1) p = 0;

	//this is ugly but works
	x = i / ROWS_PER_PAGE;
	y = (i % x) + (p*ROWS_PER_PAGE);
	x = (i - (p * PIXELS_PER_PAGE)) / ROWS_PER_PAGE;
	return sf::Vector2i(x, y);
}

void setRect_Param(uint32_t x, uint32_t y) {
	if (x >= NUMCOLS) return;
	if (x < 0) return;

	if (y >= NUMROWS) return;
	if (y < 0) return;

	//Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setFillColor(sf::Color::Red);
	//Pixels[(p * PIXELS_PER_PAGE) + ((x-p) * ROWS_PER_PAGE) + y].setOutlineThickness(10);
	Pixels[mapXYtoRect(x, y)].setOutlineColor(sf::Color::Red);
}


void setLED(int idx) {
	if (idx < 0 || idx>3) return;
	switch (idx){
	case 0:
		LED_RGBW[0].setFillColor(sf::Color::Red);
		break;
	case 1:
		LED_RGBW[1].setFillColor(sf::Color::Green);
		break;

	case 2:
		LED_RGBW[2].setFillColor(sf::Color::Blue);
		break;

	case 3:
		LED_RGBW[3].setFillColor(sf::Color::White);
		break;
	}
}

void clearLED(int idx) {
	if (idx < 0 || idx>3) return;
	switch (idx) {
	case 0:
		LED_RGBW[0].setFillColor(outlineColor);
		break;
	case 1:
		LED_RGBW[1].setFillColor(outlineColor);
		break;

	case 2:
		LED_RGBW[2].setFillColor(outlineColor);
		break;

	case 3:
		LED_RGBW[3].setFillColor(outlineColor);
		break;
	}
}

int main()
{	
	window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML OLED Emulator");
	window->setMouseCursorVisible(true);
	window->setFramerateLimit(60);
	window->setActive(true);

	sf::Font font;
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//TBpad->setOutlineThickness(1);
	TBpad->setFillColor(sf::Color(0x20,0x20,0x20));
	TBpad->setRadius(100);
	TBpad->setOrigin(100, 100);
	TBpad->setPosition(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT - TBpad->getRadius()-50));
	
	TB->setFillColor(sf::Color::Red);
	TB->setRadius(25);
	TB->setOrigin(25,25);
	TB->setPosition(TBpad->getPosition());
	sf::Vector2f TB_HOME(TBpad->getPosition());
	
	for (int i = 0; i < 4; i++) LED_RGBW.push_back(sf::RectangleShape(sf::Vector2f(20, 20)));
	for (int i = 0; i < 4; i++) LED_RGBW[i].setOrigin(sf::Vector2f(10, 10));
	for (int i = 0; i < 4; i++) LED_RGBW[i].setFillColor(outlineColor);
	LED_RGBW[0].setPosition(sf::Vector2f(TBpad->getPosition().x - 100, TBpad->getPosition().y - 100));
	LED_RGBW[1].setPosition(sf::Vector2f(TBpad->getPosition().x + 100, TBpad->getPosition().y - 100));
	LED_RGBW[2].setPosition(sf::Vector2f(TBpad->getPosition().x - 100, TBpad->getPosition().y + 100));
	LED_RGBW[3].setPosition(sf::Vector2f(TBpad->getPosition().x + 100, TBpad->getPosition().y + 100));


	mousePosf = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	
	//zero out the Pixel Display buffer
	for (int i = 0; i < 512; i++) PixelsDispBuffer[i] = 0;

	//Push Pixels onto vector array
	for (int i = 0; i < NUM_PIXELS; i++) {
		Pixels.push_back(sf::RectangleShape(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE)));
	}

	//Construct the Matrix of Pixels
	for (int p = 0; p < NUM_PAGES; p++) {
		for (int x = 0; x < NUMCOLS; x++) {
			for (int y = 0; y < ROWS_PER_PAGE; y++) {
				//printf_s("%i \r\n", (p*PIXELS_PER_PAGE)+(x*ROWS_PER_PAGE) + y); //debug the indexing. all good now
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setPosition(sf::Vector2f(PADDING_WINDOW_X + (x*DELTA_XY), PADDING_WINDOW_Y + ((p*PAGE_HEIGHT) + (y*DELTA_XY))));
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setOutlineThickness(1);
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setOutlineColor(outlineColor);
			}
		}
	}


//---------------------------------------Start Embedded Code Variables
	typedef enum {
		DPAD_READY = 0x00,
		DPAD_UP = 0x01,
		DPAD_DOWN = 0x02,
		DPAD_LEFT = 0x04,
		DPAD_RIGHT = 0x08,
	}TrackBallDirection;
	
	typedef enum {
		BTN_UP = 0x01,
		BTN_DOWN = 0x00,
	}TrackBallButton;

	uint8_t Dpad = DPAD_READY;
	bool Button = BTN_UP; //active low button

	//This is just to keep track of the trackball "position" based on user input
	uint32_t Pixel_x = 0;
	uint32_t Pixel_y = 0;

	int count = 0;

//---------------------------------------End Embedded Code Variables
	bool buttonUp = false;

//	while (!SSD1306_Init());  // initialize. blocking if OLED not detected
	SSD1306_Clear(); //clear oled display buffer
	SSD1306_DrawBitmap(0, 0, Boot, 128, 32, 1); //boot splash screen
	SSD1306_UpdateScreen(); //copy SSD1306_Buffer into PixelDispBuffer
	//SSD1306_Clear();

	//Super Loop Begin
	while (window->isOpen())
	{



		//-----------------------------------------------------Get Mouse & Keyboard inputs----------------------------------------------------------------|
		//Mouse Drawing - Writes to the SSD1306_Buffer so that we can Export the buffer to save bitmaps!
		mousePosf = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
		for (int i = 0; i < NUM_PIXELS; i++) {
			if (isMouseOverRect(&mousePosf, &Pixels[i])) {
				Pixels[i].setOutlineColor(sf::Color::Green);
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
						SSD1306_DrawPixel(mapRecttoXY(i).x, mapRecttoXY(i).y, SSD1306_COLOR_WHITE);
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					SSD1306_DrawPixel(mapRecttoXY(i).x, mapRecttoXY(i).y, SSD1306_COLOR_BLACK);
				}
			}
			else {
				Pixels[i].setOutlineColor(outlineColor);
			}
		}

		
		//mouse wheel input only? not sure how this 'event' works
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}
			else {
				if (event.type == sf::Event::MouseWheelMoved) {
					//Mouse Wheel UP
					if (event.mouseWheel.delta > 0) {
						//Do Stuff
					}
					if (event.mouseWheel.delta < 0) {
						//Do Stuff
					}
				}
			}
		}
		
		//testing the RGBW LEDS. using some.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) setLED(0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) setLED(1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) setLED(2);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) setLED(3);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) clearLED(0);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) clearLED(1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) clearLED(2);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) clearLED(3);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) SSD1306_Clear();

		//Get the Emulated TrackBall
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			Button = BTN_DOWN;
			TB->setFillColor(sf::Color::Red);
		}
		else {
			Button = BTN_UP;
			TB->setFillColor(sf::Color(0x50, 0x50, 0x50));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			Dpad |= DPAD_UP;
		}
		else {
			Dpad &= ~(DPAD_UP);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			Dpad |= DPAD_DOWN;
		}
		else {
			Dpad &= ~(DPAD_DOWN);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			Dpad |= DPAD_LEFT;
		}
		else {
			Dpad &= ~(DPAD_LEFT);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			Dpad |= DPAD_RIGHT;
		}
		else {
			Dpad &= ~(DPAD_RIGHT);
		}

		//update the trackball grapics
		float TBx = (50 * (((Dpad & DPAD_RIGHT)) >>3)) - (50 * (((Dpad & DPAD_LEFT)>> 2)));
		float TBy = (50 * (((Dpad & DPAD_DOWN)) >> 1)) - (50 * (((Dpad & DPAD_UP) >> 0)));		
		TB->setPosition(sf::Vector2f(TB_HOME.x + TBx, TB_HOME.y + TBy));
		//printf("%x \r\n", Dpad); //debug to console

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			Dpad = DPAD_READY;
		}
		

//---------------------------------------Start Embedded Code Logic
		//MCU drawing code using Trackball
		if ((Dpad & DPAD_RIGHT) && Pixel_x < NUMCOLS-1) Pixel_x++;
		if ((Dpad & DPAD_LEFT) && Pixel_x > 0) Pixel_x--;
		if ((Dpad & DPAD_UP) && Pixel_y > 0) Pixel_y--;
		if ((Dpad & DPAD_DOWN) && Pixel_y < NUMROWS-1) Pixel_y++;
	
		if (!Button) {
			if (!SSD1306_getPixel(Pixel_x, Pixel_y)) {
				SSD1306_DrawPixel(Pixel_x, Pixel_y, SSD1306_COLOR_WHITE);
			}
			else {
				SSD1306_DrawPixel(Pixel_x, Pixel_y, SSD1306_COLOR_BLACK);
			}
		}
		//END MCU drawing code using Trackball

		/*SSD1306_DrawRectangle(0, 0, 31, 31, SSD1306_COLOR_WHITE);
		count++;
		SSD1306_Puti(5, 5, count, 5);*/

		SSD1306_UpdateScreen(); //copy SSD1306_Buffer into PixelDispBuffer
//---------------------------------------End Embedded Code

		
		//update OutlineColors (used for making selected Oled Pixel easier)
		//for (int i = 0; i < NUM_PIXELS; i++) Pixels[i].setOutlineColor(outlineColor);
		setRect_Param(Pixel_x, Pixel_y);
		

		//This must be called every loop to update the rectange shapes fillColor
		for (int i = 0; i < 512; i++) {
			for (int b = 0; b < 8; b++) {
				if (PixelsDispBuffer[i] & (1 << b)) {
					Pixels[(i * 8) + b].setFillColor(onColor);
				}
				else {
					Pixels[(i * 8) + b].setFillColor(offColor);
				}
			}
		}
		
		for (auto Pixel : Pixels) window->draw(Pixel);
		window->draw(*TBpad);
		window->draw(*TB);
		for (auto led : LED_RGBW) window->draw(led);
		window->display();
	}//end update loop

	return 0;
}