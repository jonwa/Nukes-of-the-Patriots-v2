#include "Randomizer.h"

#define PI 3.1416

//Returnerar en int mellan max t.o.m min.
// 
int Randomizer::randomNr(int Max, int Min)
{
	mRandom += randTimer.getElapsedTime().asMicroseconds();
	mRandom = (int)((mRandom*sin((mRandom%90)*(PI/180))))%Max+Min;
	return std::abs((int)mRandom);
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





