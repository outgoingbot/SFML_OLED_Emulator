#ifndef __UTILFUNCS_H__
#define __UTILFUNCS_H__
//Todo: convert this to c++ so I can overload functions

#ifdef __cplusplus
extern "C" {
#endif
//map one domain to another
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
//float constrain function
float constrainFloat(float x, float min, float max);

int map(int x, int in_min, int in_max, int out_min, int out_max);


int constrain(int x, int min, int max);

float absFloat(float x);

int Rand(int low, int high);

#ifdef __cplusplus
}
#endif

#endif
