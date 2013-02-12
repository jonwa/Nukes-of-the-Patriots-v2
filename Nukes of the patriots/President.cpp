#include "president.h"
#include "ResourceHandler.h"
#include <iostream>

President::President(std::string &filename):
	mYearsElected(0)
{
	
	initializeImages(filename);
	randomStatFunc();
}

President::~President()
{

}

void President::randomStatFunc()
{
	Randomizer *randomizer;
	randomizer = Randomizer::getInstance();

	randomStats.push_back("foodPrice");
	randomStats.push_back("goodsPrice");
	randomStats.push_back("techPrice");
	randomStats.push_back("nuclearPrice");
	randomStats.push_back("spacePrice");
	randomStats.push_back("spyPrice");
	randomStats.push_back("patriotismTax");
	

	std::map<std::string, float> posStatMap;
	posStatMap.insert(std::pair<std::string,float>("foodPrice", -2.f));
	posStatMap.insert(std::pair<std::string,float>("goodsPrice", -2.f));
	posStatMap.insert(std::pair<std::string,float>("techPrice", -2.f));
	posStatMap.insert(std::pair<std::string,float>("nuclearPrice", 0.8f));
	posStatMap.insert(std::pair<std::string,float>("spacePrice",0.8f));
	posStatMap.insert(std::pair<std::string,float>("spyPrice", 0.8f));
	posStatMap.insert(std::pair<std::string,float>("patriotismTax", 1.f));

	std::map<std::string, float> negStatMap;
	negStatMap.insert(std::pair<std::string,float>("foodPrice", 2.f));
	negStatMap.insert(std::pair<std::string,float>("goodsPrice", 2.f));
	negStatMap.insert(std::pair<std::string,float>("techPrice", 2.f));
	negStatMap.insert(std::pair<std::string,float>("nuclearPrice", 1.2f));
	negStatMap.insert(std::pair<std::string,float>("spacePrice", 1.2f));
	negStatMap.insert(std::pair<std::string,float>("spyPrice", 1.2f));
	negStatMap.insert(std::pair<std::string,float>("popEatsMore", 0.1f));

	int random = ( randomizer->randomNr(randomStats.size(),0) );

	mValues.insert(std::pair<std::string,float>(randomStats[random], posStatMap.find(randomStats[random])->second));
	randomStats[random] = randomStats.back();
	randomStats.pop_back();

	random = ( randomizer->randomNr(randomStats.size(),0) );
	mValues.insert(std::pair<std::string,float>(randomStats[random], posStatMap.find(randomStats[random])->second));
	randomStats[random] = randomStats.back();
	randomStats.pop_back();

	
	randomStats.push_back("popEatsMore");
	random = ( randomizer->randomNr(randomStats.size(),0) );

	if(randomStats[random] == "patriotismTax")
	{
		randomStats[random] = randomStats.back();
		randomStats.pop_back();
		random = ( randomizer->randomNr(randomStats.size(),0) );
	}

	mValues.insert(std::pair<std::string,float>(randomStats[random], negStatMap.find(randomStats[random])->second));
	randomStats[random] = randomStats.back();
	randomStats.pop_back();

	for(std::vector<std::string>::iterator it = randomStats.begin(); it != randomStats.end(); it++)
	{
		mValues.insert(std::pair<std::string, float>((*it), 0.f));
	}

	randomStats.clear();
}



float President::getFoodPriceModifier()
{
	return mValues["foodPrice"];
}

float President::getGoodsPriceModifier()
{
	return mValues["goodsPrice"];
}

float President::getTechPriceModifier()
{
	return mValues["techPrice"];
}

float President::getNuclearPriceModifier()
{
	return mValues["nuclearPrice"] == 0 ? 1 : mValues["nuclearPrice"];
}

float President::getSpacePriceModifier()
{
	return mValues["spacePrice"] == 0 ? 1 : mValues["spacePrice"];
}

float President::getSpyPriceModifier()
{
	return mValues["spyPrice"] == 0 ? 1 : mValues["spyPrice"];
}

float President::getPatriotismTaxModifier()
{
	return mValues["patriotismTax"];
}

float President::getPopEatsMore()
{
	return mValues["popEatsMore"];
}

void President::initializeImages(std::string &path)
{
	ResourceHandler* handler = ResourceHandler::getInstance();
	mTexture = &handler->getTexture(path);
	mPortrait.setTexture(*mTexture);
}

sf::Texture* President::getTexture()
{
	return mTexture;
}