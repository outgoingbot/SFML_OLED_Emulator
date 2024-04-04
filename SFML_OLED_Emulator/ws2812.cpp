/* 
* ws2812.cpp
*
* Created: 11/27/2022 10:21:59 PM
* Author: Tower
*/

#include "ws2812.h"

// default constructor
ws2812::ws2812(uint16_t numLeds){

	_numLeds = numLeds;
	_dispBuffer = (cRGB *)calloc(_numLeds,sizeof(cRGB));
}

// default destructor
ws2812::~ws2812(){
	free(_dispBuffer);
} //~ws2812


uint32_t ws2812::getPixelColor(int n) {
	return ((uint32_t)Pixels[n].getFillColor().r << 16) | ((uint32_t)Pixels[n].getFillColor().g << 8) | (uint32_t)(Pixels[n].getFillColor().b);
}

void ws2812::setPixelColor(int n, uint32_t c) {
	uint8_t red, green, blue;
	blue = (c>>0) & 0xFF;
	green = (c >> 8) & 0xFF;
	red = (c >> 16) & 0xFF;
	Pixels[n].setFillColor(sf::Color(red, green, blue));
	
	//MASK THE UNAVAILBE LEDS
#define MASK 0
#if MASK
	//LEFT EYE
	Pixels[grid[0][NUMROWS-1-0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[0][NUMROWS - 1 - 1]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[1][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[7][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[7][NUMROWS - 1 - 1]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[6][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));

	Pixels[grid[0][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[0][NUMROWS - 1 - 1]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[1][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[7][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[7][NUMROWS - 1 - 1]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[6][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));

	//RIGHT EYE
	Pixels[grid[8][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[8][NUMROWS - 1 - 1]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[9][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[15][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[15][NUMROWS - 1 - 1]].setFillColor(sf::Color(0, 0, 0));
	Pixels[grid[14][NUMROWS - 1 - 0]].setFillColor(sf::Color(0, 0, 0));

	Pixels[grid[8][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[8][NUMROWS - 1 - 1]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[9][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[15][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[15][NUMROWS - 1 - 1]].setOutlineColor(sf::Color(0, 0, 0));
	Pixels[grid[14][NUMROWS - 1 - 0]].setOutlineColor(sf::Color(0, 0, 0));
#endif
}

/*
void ws2812::setPixelColor(int i, uint32_t c){ //input a 32-bit color. write the g,r,b to display buffer
	_dispBuffer[i].g = ((c >> 16) & 0xFF); //Green
	_dispBuffer[i].r = ((c >> 8) & 0xFF); //Red
	_dispBuffer[i].b = ((c >> 0) & 0xFF); //Blue
}

uint32_t ws2812::getPixelColor(int num){
return ((uint32_t)_dispBuffer[num].g << 16) | ((uint32_t)_dispBuffer[num].r <<  8) | _dispBuffer[num].b;	
}
*/

void ws2812::show(){
	//ws2812_setleds(_dispBuffer,_numLeds*3);
	//HACK: dont bother sending the "dummy" pixels. faster updates
	//ws2812_setleds(_dispBuffer,NUMREALLEDS*3); //only sending Real Leds data 
	window->clear(sf::Color::Black);
	for (int i = 0; i < NUMLEDS; i++) window->draw(Pixels[i]);
	window->display();
}

uint32_t ws2812::Color(uint8_t r, uint8_t g, uint8_t b) {
	return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | (uint32_t)b;
}

void ws2812::setBrightness(uint8_t arg){
	if(arg>BMAX) return;
	if(arg<BMIN) return;
	_brightness = arg;
}
uint8_t ws2812::getBrightness(){
	return _brightness;
}

void ws2812::FillScreen(uint32_t c) { //quickly set the entire screen one color
	for (uint16_t i = 0; i < _numLeds; i++) setPixelColor(i, c);
	//show();
}

void ws2812::FillColumn(int n, uint32_t c) {
	for (int m = 0; m < NUMROWS; m++) setPixelColor(grid[n][m], c);
}


//Extract characters for Scrolling text
void ws2812::printStringWithShiftL(const char* s, int shift_speed) { //Add Color??
	while (*s != '\0') {
		printCharWithShiftL(*s, shift_speed);
		s++;
	}
}

// Put extracted character on Display
void ws2812::printCharWithShiftL(char c, int shift_speed) {
	#define FONTWIDTH 3
	#define FONTHEIGHT 5
	#define FONTROW 1 //distance from Top of display to place top of character
	uint8_t mbuffer[3];   //used for scroll text
	if (c < 32) return; //error check for ascii values less than ASCII  < ' ' (space) = 32 >
	c -= 32; //align the char ascii with the CharTable
	memcpy(mbuffer, &CH[FONTWIDTH * c], FONTWIDTH); //CH + 3*c is same as &CH[3*c] //copy 3 bytes
	//getrand(randSelect);
	updateColorVars(wheelColor(RAND_AREA)); //wheelArea
	for (int j = 0; j < FONTWIDTH; j++) {
		uint8_t b = 0b00000001;
		for (int k=FONTROW; k < FONTROW+FONTHEIGHT; k++) {
			if (mbuffer[j]&b) setPixelColor(grid[NUMCOLS-1][k], Forecolor);
			b = b << 1;
		}
		HorizShiftL(1,1); //shift every column one column left (one space between letters)
		FillColumn(NUMCOLS-1, 0x000000);
		//_delay_ms(shift_speed);
		std::this_thread::sleep_for(std::chrono::milliseconds(shift_speed));
	}
	HorizShiftL(1,1); // (one blank column after each character)
}


/*
a 0 in the bit position is ON. 1 is OFF
looking at front of display the byte ordering is:
MSB-LSB
LEFT	RIGHT
Byte-0	Byte-8
Byte-1	Byte-9
Byte-2	Byte-A
Byte-3	Byte-B
Byte-4	Byte-C
Byte-5	Byte-D
Byte-6	Byte-E
Byte-7	Byte-F
eyeSelect chooses to display a bitmap on the lefteye=0 righteye=1 or both=2
*/
//write bitmap saved in PROGMEM to display buffer
void ws2812::ShowBitmap(const uint8_t* bitmap, uint32_t c, uint8_t eyeSelect){
	volatile uint8_t idxStart=0,idxStop=16;
	//enum EYE{LEFT,RIGHT,BOTH};
		switch(eyeSelect){
			case LEFTEYE:
				idxStart = 0;
				idxStop = 8;
			break;
			case RIGHTEYE:
				idxStart = 8;
				idxStop = 16;
			break;
			default: //BOTHEYE
				idxStart = 0;
				idxStop = 16;
			break;
			}
	volatile uint8_t b;
	for(int x=idxStart; x<idxStop; x++){ //loop through each byte in the entire bitmap.	
			b = 0b10000000; //reset the bit mask after each byte in the array
			if(x<8){ //left eye
				for(int i = 0; i < 8; i++){ //apply the bitmask to each bit the byte
					if(!((bitmap+x))&b) setPixelColor(grid[i][x], c); //read the bitmpa byte address and increment by x
					b = b>>1;
				}
			}else{ //right eye
				for(int i = 0; i < 8; i++){ //apply the bitmask to each bit the byte
					if(!((bitmap+x))&b) setPixelColor(grid[i+RIGHTEYE_X_START][x-8], c);
					b = b>>1;
				}
			}
	}
}




//shift right function
void ws2812::HorizShiftR (int count, uint8_t withShow) {   //Shift columns right 1 palce
	for(int steps = 0; steps<count; steps++){
		for (int x = NUMCOLS-2; x >= 0; x--) { //read one hind right
			for (int y = 0; y < NUMROWS; y++) { //read each bulb value
				setPixelColor(grid[x + 1][y], getPixelColor(grid[x][y])); //put those stored vals in place one to the right
			}
		}
		if(withShow) show();
	}
}

//shift Left function 
void ws2812::HorizShiftL (int count, uint8_t withShow) {   //Shift columns left 1 place
	for(int steps = 0; steps<count; steps++){
		for (int n = 1; n < NUMCOLS; n++) { //read one behind left
			for (int m = 0; m < NUMROWS; m++) { //read each bulb value
				setPixelColor(grid[n - 1][m], getPixelColor(grid[n][m])); //put those stored vals in place one to the left
			}
		}
		if(withShow) show();
	}
}



int ws2812::getGreen(uint32_t c){ //receives 32bit color and returns the Greed values
	return ((c >> (1 * 8)) & 0xFF); //Green
}

int ws2812::getRed(uint32_t c){ //receives 32bit color and returns the Red values
	return ((c >> (2 * 8)) & 0xFF);//Red
}

int ws2812::getBlue(uint32_t c){ //receives 32bit color and returns the blue values
	return ((c >> (0 * 8)) & 0xFF);//Blue
}


void ws2812::updateColorVars(uint32_t c){
Forecolor = c;
green = getGreen(c);
red = getRed(c);
blue = getBlue(c);
}


uint32_t ws2812::wheelColor(int select){

	if(!select) return wheelColor(Rand(0,360),Rand(BMIN,_brightness)); //0 RAND_AREA
	else return wheelColor(Rand(0,360), _brightness); //1 RAND_PERIMETER

	}

//angles: RED = 0/360, GREEN = 120, BLUE = 240
uint32_t ws2812::wheelColor(int angle, int magnitude){
	//try to map small negative (under 0) and small positive (over 360)
	if(angle>360) angle-=360;
	else if(angle<0) angle=+360;
	
	if(angle<0 || angle >360) return 0x000000; //return 0 if bad angle
	if(magnitude>BMAX) return 0x000000; //return 0 if magnitude larger than BMAX
	return Wheel(map(angle,0,360,0,magnitude*3),magnitude);
	}



// The colors are a transition r - g - b - back to r.
uint32_t ws2812::Wheel(uint32_t pos, uint32_t max) { // pos must be between 0 and 3*max
if(pos<0 || pos >3*max){
	return 0; //error check
}

if(pos >= 0 && pos < max){
	return Color(pos, max-pos, 0);
}

if(pos >= max && pos < 2*max){
	 pos -= max;
	 return Color(max-pos, 0, pos);
}

if(pos >= 2*max && pos < 3*max){
	 pos -= 2*max;
	 return Color(0, pos, max-pos);
}
return 0;
}


//adding some 2D transforms

//TODO: devise a clever soultion so that there is only 1 for-loop. first loop for each case should be merged
//change this to pass in a start size % and a end size %.   like 1.0 to 0.5 or 0.05 to 1.0 etc
void ws2812::scaleScreen(float sclPercent, uint8_t originX, uint8_t originY){ //scale the all the pixels currently on the screen.   min: 0.05 and max: 1.0 i guess
	struct cRGB dispBufferTemp[NUMLEDS]; //create a display buffer
	memcpy(dispBufferTemp, _dispBuffer, NUMLEDS*sizeof(cRGB)); //copy the current display buffer into a temp

	if(sclPercent<1){	
		for(float i=1; i>=sclPercent; i-=0.05){ //step through the scaling factors. shrink the image
			//lmd.clear(); //needed?
			FillScreen(BLACK);
			for(int x=0; x<NUMCOLS; x++){ //perform the matrix multiplication
				for(int y=0; y<NUMROWS; y++){
					int myXT = (int) (originX+((x-originX)*(i))); //scaling matrix
					int myYT = (int) (originY+((y-originY)*(i)));
					setPixelColor(grid[myXT][myYT],Color(dispBufferTemp[grid[x][y]].g,dispBufferTemp[grid[x][y]].r,dispBufferTemp[grid[x][y]].b));
				}
			}
			show();
			//_delay_ms(10);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	
	if(sclPercent>=1){
		for(float i=0.05f; i<=sclPercent; i+=0.05){ //step through the scaling factors. shrink the image
			//lmd.clear(); //needed?
			FillScreen(BLACK);
			for(int x=0; x<NUMCOLS; x++){ //perform the matrix multiplication
				for(int y=0; y<NUMROWS; y++){
					int myXT = (int) (originX+((x-originX)*(i))); //scaling matrix
					int myYT = (int) (originY+((y-originY)*(i)));
					setPixelColor(grid[myXT][myYT],Color(dispBufferTemp[grid[x][y]].g,dispBufferTemp[grid[x][y]].r,dispBufferTemp[grid[x][y]].b));
				}
			}
			show();
			//_delay_ms(10);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}	
}

//rotate the display around a point. angPercent is 0 to 1. 1 will mean a compete rotation. (1*2PI)
void ws2812::rotateScreen(float angPercent, int dir, int originX, int originY){ //rotates all pixels currently on the screen theta rads counter clockwise
	//dir: 1 for CCW, -1 for CW
	struct cRGB dispBufferTemp[NUMLEDS]; //create a display buffer
	memcpy(dispBufferTemp, _dispBuffer, NUMLEDS*sizeof(cRGB)); //copy the current display buffer into a temp
	volatile int myXT;
	volatile int myYT;
	for(float i=0; i<=2*angPercent; i+=.05){ //step though the angles
		for(int x=0; x<NUMCOLS; x++){  //perform the matrix multiplication
			for(int y=0; y<NUMROWS; y++){
				myXT = (int) (originX + ( (x-originX)*cos(M_PI*i) - (y-originY)*dir*sin(M_PI*i) ));
				myYT = (int) (originY + ( (x-originX)*dir*sin(M_PI*i) + (y-originY)*cos(M_PI*i) ));	
				if(myXT>=0 && myXT<NUMCOLS && myYT>=0 && myYT<NUMROWS){
					 setPixelColor(grid[myXT][myYT],Color(dispBufferTemp[grid[x][y]].g,dispBufferTemp[grid[x][y]].r,dispBufferTemp[grid[x][y]].b));
				}				
			}
		}
		show();
		//_delay_ms(1); //if wanted
	}
	//if(ceilf(angPercent) == angPercent) printStored(); //if angle to rotate is integer value then fix rotation error by printing the tempbuff
}


/*
void ws2812::recflectScreenXY(int xR, int yR){ //(1,1) is not reflection
	storeScreen();
	lmd.clear();
		//PollButtons();
		for(int x=0; x<numCols; x++){  //perform the matrix multiplication
			for(int y=0; y<numRows; y++){
					int myXT = (int) (myOffset+ ((x-myOffset)*(xR))); //scaling matrix
					int myYT = (int) (myOffset+((y-myOffset)*(yR)));
				SetLEDxy(myXT,myYT,buffTemp[x][y]);
			}
		}
}


void uint32_t ws2812::storeScreen(){ //copy every mono led state to a buffer called bitmap
	for(int x=0; x<16; x++){
		for(int y=0; y<16; y++){
			buffTemp[x][y] = GetLEDxy(x,y);
		}
	}
}


void uint32_t ws2812::printStored(){ //print the buffer called bitmap
	for(int x=0; x<16; x++){
		for(int y=0; y<16; y++){
			SetLEDxy(x,y,buffTemp[x][y]);
		}
	}
	lmd.display();
}
*/