#include "Randomizer.h"
#include <vector>
#include <algorithm>
#include <iostream>

//Returnerar en int mellan max t.o.m min.
// 
int Randomizer::randomNr(int Max, int Min)
{
	int random = randTimer.getElapsedTime().asMicroseconds()%Max+Min;
	 
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





