#include "Randomizer.h"

#define PI 3.1416

//Returnerar en int mellan max t.o.m min.
// 
int Randomizer::randomNr(int Max, int Min)
{
	mRandom += randTimer.getElapsedTime().asMicroseconds() + sin(mRandom*(PI/180))*7231;
	int random = (mRandom%Max) + Min;
	return random;
}


//singleton
Randomizer* Randomizer::instance = NULL;
Randomizer* Randomizer::getInstance()
{
	if(instance == NULL)
	{
		instance = new Randomizer();
	}
	return instance;
}





