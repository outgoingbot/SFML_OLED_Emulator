#ifndef __TYPES__
#define __TYPES__
#include <SFML/Graphics.hpp>

typedef struct ballparams {
	sf::Vector2f BlackHolePostion = { 10,10 };
	int radioactivity = 2; //maximum launch velocity
	int drawVectorField = 0; //vectorfield lines drawn or not
	float fieldStrength = 0.2; //strength of the field
} params_t;



#endif // !__TYPES__
