/*
 * light weight WS2812 lib include
 *
 * Version 2.3  - Nev 29th 2015
 * Author: Tim (cpldcpu@gmail.com) 
 *
 * Please do not change this file! All configuration is handled in "ws2812_config.h"
 *
 * License: GNU GPL v2+ (see License.txt)
 +
 */ 

#ifndef LIGHT_WS2812_H_
#define LIGHT_WS2812_H_
#ifdef __cplusplus
extern "C" {
	#endif
	

#include <stdint.h>


struct cRGB  { uint8_t g; uint8_t r; uint8_t b; };


#ifdef __cplusplus
}
#endif

#endif /* LIGHT_WS2812_H_ */