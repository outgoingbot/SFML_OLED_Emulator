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
//#include "Hardware.h"
//#include "ws2812.h"


#define WINDOW_WIDTH 1920-200
#define WINDOW_HEIGHT 1080-200

#define NUM_PAGES 4
#define NUMCOLS 128
#define NUMROWS 32
#define NUMLEDS NUMCOLS*NUMROWS //used for the buffer size

#define ROWS_PER_PAGE NUMROWS/NUM_PAGES

#define PIXEL_WIDTH 5
#define PIXEL_HEIGHT 5

#define PAGE_HEIGHT 6 * 8 //will need to add some spacing

#define DELTA_X 6//WINDOW_WIDTH/NUMCOLS
#define DELTA_Y 6//WINDOW_HEIGHT/NUMROWS

#define M_PI 3.141592653589793

//-----------------------------------Global Variables--------------------------------------

//Graphics
sf::RenderWindow* window = nullptr;

//Make Pixels array
//std::vector<std::vector<sf::RectangleShape> > Pixels(NUMROWS,std::vector<sf::RectangleShape>(NUMCOLS)); // cant figure out how to make 2d object vector
std::vector<sf::RectangleShape> Pixels;

//--------------------------------------User Function Definitions--------------------------------------


int main()
{	

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
		Pixels.push_back(sf::RectangleShape(sf::Vector2f(PIXEL_WIDTH, PIXEL_HEIGHT)));
	}

	for (int p = 0; p < NUM_PAGES; p++) {
		for (int x = 0; x < NUMCOLS; x++) {
			for (int y = 0; y < 8; y++) {
				std::cout << ((p+1)*(x * 8) + y) << std::endl;
				//Pixels[(p * 128) + (x * 1) + y].setPosition(sf::Vector2f(x*DELTA_X, (p*PAGE_HEIGHT) + (y*DELTA_Y)));
				//Pixels[(p * 128) + (x * 1) + y].setFillColor(sf::Color::Blue);
				//Pixels[(p * 128) + (x * 1) + y].setOutlineThickness(1);
				//Pixels[(p * 128) + (x * 1) + y].setOutlineColor(sf::Color(0x80, 0x80, 0x80));
			}
		}

	}


	//Construct the Matrix of Pixels
	/*
	#define X_OFFSET 0
	#define Y_OFFSET 0
	float xPos = X_OFFSET;
	float yPos = PIXEL_HEIGHT + Y_OFFSET;
	for (int y = 0; y < NUMROWS; y++) {
		for (int x = 0; x < NUMCOLS; x++) {
			Pixels[(y*NUMCOLS) + x].setPosition(sf::Vector2f(xPos, yPos));
			Pixels[(y*NUMCOLS) + x].setOutlineThickness(1);
			Pixels[(y*NUMCOLS) + x].setOutlineColor(sf::Color(0x80,0x80,0x80));
			Pixels[(y*NUMCOLS) + x].setFillColor(sf::Color(0x00, 0x00, 0x64));
			xPos += DELTA_X;			
		}
		xPos = X_OFFSET;
		yPos += (DELTA_Y - Y_OFFSET);
	}
	*/


	//for(auto Pixel : Pixels) Pixel.setFillColor(sf::Color(0x00, 0x128, 0x0));

	//Pixels[(y*NUMCOLS) + x].setFillColor(sf::Color(0x128, 0x00, 0x20));

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

		for (int i = 0; i < NUMLEDS; i++) window->draw(Pixels[i]);
		window->display();
	}//end update loop

	return 0;
}