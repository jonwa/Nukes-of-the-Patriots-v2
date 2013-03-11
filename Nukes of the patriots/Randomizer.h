#pragma once

#include <SFML\System.hpp>
#include <algorithm>
#include <math.h>

//SlumptalsGenerator av Joakim Stark
//randomNr() returnerar en float mellan Max t.o.m Min argumentet.
//(Reset på timern körs inte för tillfället någonstans)
//Använder singleton mönstret, som gör att man bara kan skapa en instans av klassen.
//	Användningsexempel:
//	Randomizer *randomizer;
//	randomizer = Randomizer::getInstance();
//	int random = randomizer->randomNr(10,1);


class Randomizer
{
public:
	~Randomizer();
	int randomNr(int Max, int Min);
	static Randomizer* getInstance();


protected:
	
	Randomizer(const Randomizer&);
	Randomizer& operator= (const Randomizer&);
private:
	Randomizer():mRandom(0){}
	static Randomizer* instance;
	sf::Clock randTimer;
	unsigned long mRandom;

};