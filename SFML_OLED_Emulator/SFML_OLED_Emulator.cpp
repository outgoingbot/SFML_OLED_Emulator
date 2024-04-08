/*
SSD1306 OLED Emulator

keyboard arrow keys move pixel selection.
space with toggle the pixel color - bouncy atm :(

escape clears the LL buffer (which in turn clears the local buffer)
1,2,3,4 turn ON RGBW Leds
LShift + 1,2,3,4 turn OFF RGBW Leds
 
 mouse left click can also setPixels and mouse Righgt click clears them

ctrl + s save the bitmap (byte array) to a file on the system
ctrl + l load the bitmap (byte array) to the display ssd1306_buffer[] (non-local) on the system

Instructions:
Draw subsized bitmaps at top left origin. extract the array from the console...for now

*/

#include <iomanip>
//#include <iostream>
#include <locale>

#include <fstream> 
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
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
#define FPS 60
//adjust where the "OLED' is placed reletive to the window Origin
#define PADDING_WINDOW_X 70
#define PADDING_WINDOW_Y 50

//OLED PARAMS
#define NUMCOLS 128
#define NUMROWS 32
#define NUM_PAGES 4
#define BUFFER_SIZE  (NUMCOLS*NUMROWS / 8)

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

sf::Text* DebugText = new sf::Text();
bool buttonUp = false;

//User Input
sf::Vector2f mousePosf;

//Make Pixels array
std::vector<sf::RectangleShape> Pixels;
sf::Color outlineColor = sf::Color(50, 50, 50);
sf::Color onColor = sf::Color(66, 245, 212);
sf::Color offColor = sf::Color::Black;

#ifdef __cplusplus
extern "C" {
	uint8_t PixelsDispBuffer[BUFFER_SIZE]; //must match the SSD1306 Buffer size
}
#endif

//function prototyping
int saveFile();
int loadFile();
bool isMouseOverRect(sf::Vector2f* mousePosition, sf::RectangleShape* RS);
int mapXYtoRect(uint32_t x, uint32_t y);
sf::Vector2i mapRecttoXY(uint32_t i);
void setLED(int idx);
void clearLED(int idx);
void setRect_Param(uint32_t x, uint32_t y);
//function prototypes




//convert Oled Buffer Layout to bitmap layout
//must use SSD1306_drawBitmap() to load the output of this
void transpose(uint8_t source[], uint8_t dest[]) {
	uint8_t temp[BUFFER_SIZE];
	uint16_t sourceIdx = 0;
	uint16_t destIdx = 0;

	for (int i = 0; i < BUFFER_SIZE; i++) temp[i] = 0;

	for (uint16_t col = 0; col < 4; col++) { //index through each array
		for (uint8_t bit = 0; bit < 8; bit++) {
			for (uint16_t row = 0; row < 16; row++) { //index through each array
				for (uint8_t byteIdx = 0; byteIdx < 8; byteIdx++) {
					sourceIdx = (col*128)+(row * 8)+byteIdx;
					if (source[sourceIdx] & (1 << bit)) {
						temp[destIdx] |= (1 << (7 - byteIdx));
					}
				}
				destIdx++;
			}
			
		}
	}

	for (int i = 0; i < BUFFER_SIZE; i++) dest[i] = temp[i]; //copy back

}

int main()
{	
	window = new sf::RenderWindow (sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML OLED Emulator");
	window->setMouseCursorVisible(true);
	window->setFramerateLimit(FPS);
	window->setActive(true);

	sf::Font font;
	if (!font.loadFromFile("../res/arial.ttf")) {
		printf("Error loading Font");
		system("pause");
	}
	// select the font
	DebugText->setFont(font); // font is a sf::Font
	DebugText->setPosition(sf::Vector2f(10, 10));
	DebugText->setString("Empty ");// set the string to display
	DebugText->setCharacterSize(20);
	//Setup the TrackBall
	TBpad->setFillColor(sf::Color(0x20,0x20,0x20));
	TBpad->setRadius(100);
	TBpad->setOrigin(100, 100);
	TBpad->setPosition(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT - TBpad->getRadius()-50));
	
	TB->setFillColor(sf::Color::Red);
	TB->setRadius(25);
	TB->setOrigin(25,25);
	TB->setPosition(TBpad->getPosition());
	sf::Vector2f TB_HOME(TBpad->getPosition());
	
	//Setup the TrackBall LEDs
	for (int i = 0; i < 4; i++) LED_RGBW.push_back(sf::RectangleShape(sf::Vector2f(20, 20)));
	for (int i = 0; i < 4; i++) LED_RGBW[i].setOrigin(sf::Vector2f(10, 10));
	for (int i = 0; i < 4; i++) LED_RGBW[i].setFillColor(outlineColor);
	LED_RGBW[0].setPosition(sf::Vector2f(TBpad->getPosition().x - 100, TBpad->getPosition().y - 100));
	LED_RGBW[1].setPosition(sf::Vector2f(TBpad->getPosition().x + 100, TBpad->getPosition().y - 100));
	LED_RGBW[2].setPosition(sf::Vector2f(TBpad->getPosition().x - 100, TBpad->getPosition().y + 100));
	LED_RGBW[3].setPosition(sf::Vector2f(TBpad->getPosition().x + 100, TBpad->getPosition().y + 100));

	//initialize mouse position variable
	mousePosf = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	
	//zero out the Pixel Display buffer
	for (int i = 0; i < BUFFER_SIZE; i++) PixelsDispBuffer[i] = 0;

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

	typedef enum {
		LOCKED = 0x01,
		NOTLOCKED = 0x00,
	}TrackBallLockState;

	typedef struct {
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;
		bool button = false;
	}TrackBallLock_t;

	TrackBallLock_t TB_lock;

	uint8_t Dpad = DPAD_READY;
	bool Button = BTN_UP; //active low button

	
	//This is just to keep track of the trackball "position" based on user input
	uint32_t Pixel_x = 0;
	uint32_t Pixel_y = 0;

	int count = 0;
	
	//trying to keep track of time
	#define ZERO_VEL 0
	sf::Vector2f velocity = {0,0};
	float TestDeltaT = 1/FPS;
//---------------------------------------End Embedded Code Variables


//------------------------------------Start Embedded Splash
	SSD1306_Init();
	SSD1306_Clear(); //clear oled display buffer
	SSD1306_DrawBitmap(0, 0, Boot, 128, 32, SSD1306_COLOR_WHITE); //boot splash screen
	SSD1306_GotoXY(0, 0);
	//SSD1306_DrawBitmap(128-16, 0, Bat, 16, 8, SSD1306_COLOR_WHITE); //boot splash screen
	//SSD1306_DrawRectangle(0, 0, 31, 31, SSD1306_COLOR_WHITE);
	//SSD1306_Puti(5, 5, 9999, 5);	
	//SSD1306_GotoXY(0, 0);
	//SSD1306_DrawRectangle(0, 0, 31, 31, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen(); //copy SSD1306_Buffer into PixelDispBuffer
//-----------------------------------End Embedded Splash

	//Super Loop Begin
	while (window->isOpen())
	{
		auto startTime = std::chrono::high_resolution_clock::now();

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

		//escape will erase the SSD1306_buffer
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) SSD1306_Clear();

		//save the buffer to a file
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			while (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::S)); //shitty way to not spam save. will add a lock later
			saveFile();
		}
		//load the buffer from a file
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
			while (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::L)); //shitty way to not spam save. will add a lock later
			loadFile();
		}

#define SLEEP_TIME 50
		//Get the Emulated TrackBall
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && TB_lock.button == NOTLOCKED) {// && button lock set
			TB_lock.button = LOCKED;
			Sleep(SLEEP_TIME); //trying to slow down the keyboard inputs
			Button = BTN_DOWN;
			TB->setFillColor(sf::Color::Red);
		}
		else { //Button lock release
			TB_lock.button = NOTLOCKED;
			Button = BTN_UP;
			TB->setFillColor(sf::Color(0x255, 0x255, 0x255));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && TB_lock.up == NOTLOCKED) {
			//velocity.y -= 0.1f;
			TB_lock.up = LOCKED;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) Sleep(SLEEP_TIME); //trying to slow down the keyboard inputs
			Dpad |= DPAD_UP;
		}
		else {
			TB_lock.up = NOTLOCKED;
			Dpad &= ~(DPAD_UP);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && TB_lock.down == NOTLOCKED) {
			TB_lock.down = LOCKED;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) Sleep(SLEEP_TIME); //trying to slow down the keyboard inputs
			Dpad |= DPAD_DOWN;
		}
		else {
			TB_lock.down = NOTLOCKED;
			Dpad &= ~(DPAD_DOWN);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && TB_lock.left == NOTLOCKED) {
			TB_lock.left = LOCKED;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) Sleep(SLEEP_TIME); //trying to slow down the keyboard inputs
			Dpad |= DPAD_LEFT;
		}
		else {
			TB_lock.left = NOTLOCKED;
			Dpad &= ~(DPAD_LEFT);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && TB_lock.right == NOTLOCKED) {
			TB_lock.right = LOCKED;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) Sleep(SLEEP_TIME); //trying to slow down the keyboard inputs
			Dpad |= DPAD_RIGHT;
		}
		else {
			TB_lock.right = NOTLOCKED;
			Dpad &= ~(DPAD_RIGHT);
		}

		//update the trackball graphics
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
		//if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
		//while (sf::Keyboard::isKeyPressed(sf::Keyboard::Space));
			if (!SSD1306_getPixel(Pixel_x, Pixel_y)) {
				SSD1306_DrawPixel(Pixel_x, Pixel_y, SSD1306_COLOR_WHITE);
			}
			else {
				SSD1306_DrawPixel(Pixel_x, Pixel_y, SSD1306_COLOR_BLACK);
			}
		}
		//END MCU drawing code using Trackball
		
		/*write menu code here*/
	

		//count++;
		//SSD1306_Puti(5, 5, count, 5);
		
		SSD1306_UpdateScreen(); //copy SSD1306_Buffer into PixelDispBuffer
//---------------------------------------End Embedded Code


//---------------------UPDATE NON-MCU UI Elements
		//update OutlineColors (used for making selected Oled Pixel easier)
		setRect_Param(Pixel_x, Pixel_y);
		//show keyboard higlighted pixel location (x,y)
		char c[32];
		sprintf_s(c, 32, "(%i , %i)", Pixel_x, Pixel_y);
		DebugText->setString(c);
//---------------------UPDATE NON-MCU UI Elements


		//This must be called every loop to update the rectange shapes fillColor
		for (int i = 0; i < BUFFER_SIZE; i++) {
			for (int b = 0; b < 8; b++) {
				if (PixelsDispBuffer[i] & (1 << b)) {
					Pixels[(i * 8) + b].setFillColor(onColor);
				}
				else {
					Pixels[(i * 8) + b].setFillColor(offColor);
				}
			}
		}
		window->clear();
		for (auto Pixel : Pixels) window->draw(Pixel);
		window->draw(*TBpad);
		window->draw(*TB);
		for (auto led : LED_RGBW) window->draw(led);
		window->draw(*DebugText);
		window->display();
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);
		TestDeltaT = (float)duration.count() / 1000.f;
		//printf("Delta T %f\r\n", TestDeltaT);
		
	}//end update loop

	return 0;
}
//---------------------------END MAIN







//--------------------------------------User Function Definitions--------------------------------------

int saveFile() {
	int size = BUFFER_SIZE; //num bytes to write
	// Open for Write
	std::ofstream outfile("OLED_bitmap.txt", std::ios::binary | std::ios::out);
	if (!outfile.is_open()) {
		std::cerr << "Failed to open file for writing.\n";
		return 1;
	}
	uint8_t buf[BUFFER_SIZE];
	printf("Converting bitmap format\r\n");
	transpose(SSD1306_Buffer, buf);

	printf("Saving bitmap for Emulator\r\n");
	outfile.width(1);
	outfile.write((char*)buf, size); // Writing the array elements to the file 
	outfile.close(); // Closing the file 
	

	//----------------------Code File----------------------
	// Open bitmap hex array for Write
	std::ofstream outfileCode("OLED_bitmap_code.txt");
	if (!outfileCode.is_open()) {
		std::cerr << "Failed to open file for writing.\n";
		return 1;
	}
	// Writing the array elements to the file 
	outfileCode << "const unsigned char Bitmap [] = { \r";
	char charbuf[32];
	for (int i = 0; i < size; i++) {
		sprintf_s(charbuf, 32, "0x%02x, ", SSD1306_Buffer[i]);
		outfileCode << charbuf;
		if (!((i + 1) % 16)) outfileCode << "\r";
	}
	outfileCode << "};\r";
	printf("Saving bitmap for MCU code \r\n");
	outfileCode.close();
}


int loadFile() {
	uint8_t temp[BUFFER_SIZE];
	int size = BUFFER_SIZE;
	printf("\r\nOpening File for Read\r\n");
	std::ifstream infile("OLED_bitmap.txt", std::ios::binary | std::ios::in); // Opening the file in read mode 
	infile.read((char*)temp, size); //read the array
	infile.close(); // Closing the file

	SSD1306_GotoXY(0, 0);
	SSD1306_DrawBitmap(0, 0, temp, 128, 32, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen(); //do i need to call update screen here?
	// Displaying the loaded contents to the Console
	printf("SSD1306_Buffer[] Loaded: \r\n");
	for (int i = 0; i < size; i++) {
		printf("%2x ", SSD1306_Buffer[i]);
		if (!((i + 1) % 32)) printf("\r\n");
	}
	return 0;
}



bool isMouseOverRect(sf::Vector2f* mousePosition, sf::RectangleShape* RS) {
	if (mousePosition->x > RS->getPosition().x && mousePosition->x < RS->getPosition().x + RS->getSize().x) {
		if (mousePosition->y > RS->getPosition().y && mousePosition->y < RS->getPosition().y + RS->getSize().y) {
			return true;
		}
	}
	return false;
}


int mapXYtoRect(uint32_t x, uint32_t y) {
	if (x >= NUMCOLS || x < 0) return 0;
	if (y >= NUMROWS || y < 0) return 0;
	//map 2d coordinates to rectange array Pixels
	 uint32_t p = y / ROWS_PER_PAGE;
	//not sure why I need to shift the x axis back by p.
	return ((p * PIXELS_PER_PAGE) + ((x - p) * ROWS_PER_PAGE) + y);
}


//given an rectangle vector index #, return the x,y position
sf::Vector2i mapRecttoXY(uint32_t i) {
	if (i > BUFFER_SIZE*8 || i < 0) return sf::Vector2i(0, 0);
	uint32_t p, x, y;
	p = i / PIXELS_PER_PAGE;
	x = (i - (p * PIXELS_PER_PAGE)) / ROWS_PER_PAGE;
	y = (i % 8) + (p*ROWS_PER_PAGE);
	return sf::Vector2i(x, y);
}


//RGBW LED functions
void setLED(int idx) {
	if (idx < 0 || idx>3) return;
	switch (idx) {
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


//when the mouse is over a rectangle Do something
void setRect_Param(uint32_t x, uint32_t y) {
	if (x >= NUMCOLS || x < 0) return;
	if (y >= NUMROWS || y < 0) return;

	//Pixels[(p * PIXELS_PER_PAGE) + (x * ROWS_PER_PAGE) + y].setFillColor(sf::Color::Red);
	//Pixels[(p * PIXELS_PER_PAGE) + ((x-p) * ROWS_PER_PAGE) + y].setOutlineThickness(10);
	Pixels[mapXYtoRect(x, y)].setOutlineColor(sf::Color::Red); //change color of border
}

