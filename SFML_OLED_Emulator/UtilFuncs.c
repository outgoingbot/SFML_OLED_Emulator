#include "UtilFuncs.h"
#include <stdlib.h>

//map one domain to another
inline float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//float constrain function
float constrainFloat(float x, float min, float max){
	if (x>max) return max;
	if (x<min) return min;
	return x;
}

int map(int x, int in_min, int in_max, int out_min, int out_max) { //uint32_t Map function
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int constrain(int x, int min, int max){ //uint32_t constrain function
	if (x>max) return max;
	if (x<min) return min;
	return x;
}


float absFloat(float x){
	if (x>=0) return x;
	if (x<0) return -x;
	return x;
}

int Rand(int low, int high){
	return (rand() % (high - low)) + low; //get a random number in the range low to high. i.e (5, 10) is 5 to 9
}
