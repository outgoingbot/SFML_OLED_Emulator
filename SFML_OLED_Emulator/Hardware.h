#pragma once
//Starios 2427 display

#define LEFTEYE_X_START 0
#define LEFTEYE_X_END 7
#define RIGHTEYE_X_START 8
#define RIGHTEYE_X_END 15

#define NUMCOLS 128
#define NUMROWS 32
#define NUMLEDS NUMCOLS*NUMROWS //used for the buffer size
#define NUMREALLEDS 128 //used for the transmission size

#define BMAX 255 //if you change these some functions will break!
#define BMED 224
#define BMIN 128

const unsigned char grid[NUMCOLS][NUMROWS] = {
	{0,16,32,48,64,80,96,112}, //Column 0
	{1,17,33,49,65,81,97,113}, //Column 1
	{2,18,34,50,66,82,98,114}, //Column 2
	{3,19,35,51,67,83,99,115}, //Column 3
	{4,20,36,52,68,84,100,116}, //Column 4
	{5,21,37,53,69,85,101,117}, //Column 5
	{6,22,38,54,70,86,102,118}, //Column 6
	{7,23,39,55,71,87,103,119}, //Column 7

	{8,24,40,56,72,88,104,120}, //Column 8
	{9,25,41,57,73,89,105,121}, //Column 9
	{10,26,42,58,74,90,106,122}, //Column A
	{11,27,43,59,75,91,107,123}, //Column B
	{12,28,44,60,76,92,108,124}, //Column C
	{13,29,45,61,77,93,109,125}, //Column D
	{14,30,46,62,78,94,110,126}, //Column E
	{15,31,47,63,79,95,111,127} //Column F
};