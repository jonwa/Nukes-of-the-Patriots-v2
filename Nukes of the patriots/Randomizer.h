#pragma once

#include <SFML\System.hpp>
#include <vector>

//SlumptalsGenerator av Joakim Stark
//randomNr() returnerar en float mellan Max t.o.m Min argumentet.
//(Reset p� timern k�rs inte f�r tillf�llet n�gonstans)
//Anv�nder singleton m�nstret, som g�r att man bara kan skapa en instans av klassen.
//	Anv�ndningsexempel:
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
	Randomizer(){}
	static Randomizer* instance;
	 sf::Clock randTimer;

};