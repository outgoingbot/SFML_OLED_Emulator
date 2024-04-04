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
#include <RS232Comm.h>
#include <cmath>
#include "Hardware.h"
#include "ws2812.h"

#define WINDOW_WIDTH 2600
#define WINDOW_HEIGHT 1080

#define PIXEL_WIDTH 10
#define PIXEL_HEIGHT 10

#define M_PI 3.141592653589793

//-----------------------------------Global Variables--------------------------------------

//Graphics
sf::RenderWindow* window = nullptr;

//Make Pixels array
//std::vector<std::vector<sf::RectangleShape> > Pixels(NUMROWS,std::vector<sf::RectangleShape>(NUMCOLS)); // cant figure out how to make 2d object vector
std::vector<sf::RectangleShape> Pixels;
ws2812 panel(NUMLEDS);

char charArray[256] = "Empty"; //graphics text buffer sprintf

//holds the 8bit audio values from the serial port
#define HIGHT 10
#define HOFFSET 8 //minum value for hightThress (really should be equation (highThresh[i]-lowThresh > HOFFSET)


//--------------------------------------User Function Definitions--------------------------------------





int main()
{	

	//sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, 1080), "SFML Playground");
	window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML OLED Emulator");
	sf::Vector2f mousePos = (sf::Vector2f) sf::Mouse::getPosition(*window);
	window->setMouseCursorVisible(true);
	window->setFramerateLimit(200);
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
		Pixels.push_back(sf::RectangleShape(sf::Vector2f(PIXEL_WIDTH, PIXEL_HEIGHT)));
	}

	panel.setBrightness(BMAX);

	//Construct the Matrix of Pixels
	#define DELTA_X WINDOW_WIDTH/NUMCOLS
	#define DELTA_Y WINDOW_HEIGHT/NUMROWS
	#define X_OFFSET 10	
	#define Y_OFFSET 15
	float xPos = X_OFFSET;
	float yPos = PIXEL_HEIGHT + Y_OFFSET;
	for (int y = 0; y < NUMROWS; y++) {
		for (int x = 0; x < NUMCOLS; x++) {
			Pixels[(y*NUMCOLS) + x].setPosition(sf::Vector2f(xPos, yPos));
			Pixels[(y*NUMCOLS) + x].setOutlineThickness(1);
			Pixels[(y*NUMCOLS) + x].setOutlineColor(sf::Color(0x80,0x80,0x80));
			//Pixels[(x*NUM_ROWS)+y].setFillColor(sf::Color(0x00, 0x00, 0xFF));
			panel.setPixelColor(grid[x][y], 0xAA0000);
			xPos += DELTA_X;			
		}
		xPos = X_OFFSET;
		yPos += (DELTA_Y - Y_OFFSET);
	}


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

		//for (int i = 0; i < NUMLEDS; i++) window->draw(Pixels[i]);
		
		panel.show();
		//window->display();
	}//end update loop

	return 0;
}