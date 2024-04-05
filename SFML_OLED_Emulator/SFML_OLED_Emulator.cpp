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
#define WINDOW_WIDTH 1920-200
#define WINDOW_HEIGHT 1080-200

//OLED PARAMS
#define NUMCOLS 128
#define NUMROWS 32
#define NUM_PAGES 4
#define NUMLEDS NUMCOLS*NUMROWS //used rectange vector

#define ROWS_PER_PAGE NUMROWS/NUM_PAGES
#define PIXELS_PER_PAGE ROWS_PER_PAGE * NUMCOLS
#define PIXEL_SIZE 5
//#define PIXEL_HEIGHT 5

#define DELTA_XY 10//WINDOW_WIDTH/NUMCOLS
#define PADDING_WINDOW_X 50
#define PADDING_WINDOW_Y 50

#define PAGE_HEIGHT DELTA_XY * ROWS_PER_PAGE //will need to add some spacing



//-----------------------------------Global Variables--------------------------------------

//Graphics
sf::RenderWindow* window = nullptr;

//Make Pixels array
//std::vector<std::vector<sf::RectangleShape> > Pixels(NUMROWS,std::vector<sf::RectangleShape>(NUMCOLS)); // cant figure out how to make 2d object vector
std::vector<sf::RectangleShape> Pixels;

uint8_t PixelsDispBuffer[512]; //must match the SSD1306 Buffer size
//--------------------------------------User Function Definitions--------------------------------------


int main()
{	

	//zero out the buffer
	for (int i = 0; i < 512; i++) PixelsDispBuffer[i] = 0;

	window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML OLED Emulator");
	sf::Vector2f mousePos = (sf::Vector2f) sf::Mouse::getPosition(*window);
	window->setMouseCursorVisible(true);
	window->setFramerateLimit(60);
	sf::Font font;
	if (!font.loadFromFile("../res/Pumpkin_Pancakes.ttf")) {
		printf("Error loading Font");
		system("pause");
	}

	//set FPS
	window->setFramerateLimit(60); //seriously reduces the CPU/GPU utilization
	//window.setVerticalSyncEnabled(true);
	window->setActive(true);



	//Push Pixels onto vector array
	for (int i = 0; i < NUMLEDS; i++) {
		Pixels.push_back(sf::RectangleShape(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE)));
	}

	
	//Construct the Matrix of Pixels
	for (int p = 0; p < NUM_PAGES; p++) {
		for (int x = 0; x < NUMCOLS; x++) {
			for (int y = 0; y < ROWS_PER_PAGE; y++) {
				//printf_s("%i \r\n", (p*PIXELS_PER_PAGE)+(x*ROWS_PER_PAGE) + y); //debug the indexing. all good now
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setPosition(sf::Vector2f(PADDING_WINDOW_X + (x*DELTA_XY), PADDING_WINDOW_Y + ((p*PAGE_HEIGHT) + (y*DELTA_XY))));
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setFillColor(sf::Color::Blue);
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setOutlineThickness(1);
				Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setOutlineColor(sf::Color(0x20, 0x20, 0x20));
			}
		}

	}

//	while (!SSD1306_Init());  // initialize. blocking if OLED not detected
	//SSD1306_Clear(); //clear oled display buffer
	
	//SSD1306_UpdateScreen(); //this is not working. i exposed (static -> extern) the static SSD1306_Buffer just to do some tests
	SSD1306_DrawBitmap(0, 0, Boot, 128, 32, 1); //boot splash screen
	for (int i = 0; i < 512; i++) PixelsDispBuffer[i] = SSD1306_Buffer[i];
	
	for (int i = 0; i < 512; i++) {
		for (int b = 0; b < 8; b++) {
			if (PixelsDispBuffer[i] & (1 << b)) {
				Pixels[(i*8)+b].setFillColor(sf::Color::Blue);
			}
			else {
				Pixels[(i * 8) + b].setFillColor(sf::Color::Black);
			}
	}
		}




	//SSD1306_UpdateScreen();

	//Super Loop Begin
	while (window->isOpen())
	{
		//-----------------------------------------------------Get Mouse & Keyboard inputs----------------------------------------------------------------|
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			//Do Stuff	
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			//Do Stuff
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
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			//Do Stuff
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			//Do Stuff
		}
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) DebugText.move(10,0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) DebugText.move(-10, 0);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) DebugText.move(0, -10);
		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) DebugText.move(0, 10);



		for (auto Pixel : Pixels) window->draw(Pixel);
		window->display();
	}//end update loop

	return 0;
}