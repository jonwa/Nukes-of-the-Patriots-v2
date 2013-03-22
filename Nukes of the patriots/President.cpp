#include "president.h"
#include "ResourceHandler.h"
#include <iostream>
#include "tinyxml2.h"
#include <sstream>
#include "Timer.h"

static std::string histories[] = {
	""
};

static std::map<std::string, std::string> biographyMap;
static std::map<std::string, std::pair<int, int>> animationMap;
static std::map<std::string, std::vector<sf::Texture*>> textures;
static Timer* resetTimer;

void initBiographyMap()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("XML/PresidentText.xml");

	if(doc.Error())
		std::cout << "Fel!";
	
	tinyxml2::XMLElement *presidentsNode = doc.FirstChildElement("PresidentText");
	tinyxml2::XMLElement *president = presidentsNode->FirstChildElement("President");
	while(president != NULL)
	{
		std::string name = president->Attribute("name");
		const char* _frames = president->Attribute("frames");
		const char* _timeout = president->Attribute("timeout");
		const char* _directory = president->Attribute("directory");
		int frames = 0;
		int timeout = 0;
		if(_frames != NULL)
			frames = std::atoi(_frames);
		if(_timeout != NULL)
			timeout = std::atoi(_timeout);
		std::string text = president->GetText();
		animationMap.insert(std::pair<std::string, std::pair<int, int>>(name, std::pair<int, int>(frames, timeout)));
		biographyMap.insert(std::pair<std::string, std::string>(name, text));
		if(_frames != NULL && _timeout != NULL && _directory != NULL)
		{
			textures[name] = std::vector<sf::Texture*>();
			for(int i = 0; i < frames; i++)
			{
				std::stringstream str;
				str<<_directory<<name<<"-animation"<<1+i;
				textures[name].push_back(&ResourceHandler::getInstance()->getTexture(str.str()));
			}
		}
		president = president->NextSiblingElement("President");
	}

}

President::President(std::string &filename, std::string &name):
	mName(name),
	mYearsElected(0),
	mCurrentFrame(0),
	mFrames(0),
	mTimeOut(0)
{
	if(biographyMap.empty())
		initBiographyMap();
	
	mFrames		= animationMap[name].first;
	mTimeOut	= animationMap[name].second;
	initializeImages(filename);

	randomStatFunc();
}

President::~President()
{

}

std::string& President::getBiography()
{
	return biographyMap.find(mName)->second;
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
	
	std::map<std::string, std::string> posStatsText;
	posStatsText.insert(std::pair<std::string, std::string>("foodPrice", "Food Price -3"));
	posStatsText.insert(std::pair<std::string, std::string>("goodsPrice", "Goods Price -3"));
	posStatsText.insert(std::pair<std::string, std::string>("techPrice", "Tech Price -3"));
	posStatsText.insert(std::pair<std::string, std::string>("nuclearPrice", "Nuclear weapons \n20% cheaper"));
	posStatsText.insert(std::pair<std::string, std::string>("spacePrice", "Space program \n20% cheaper"));
	posStatsText.insert(std::pair<std::string, std::string>("spyPrice", "Spy network \n20% cheaper"));
	posStatsText.insert(std::pair<std::string, std::string>("patriotismTax", "Tax cuts give \n+1 extra patriotism"));

	std::map<std::string, std::string> negStatsText;
	negStatsText.insert(std::pair<std::string, std::string>("foodPrice", "Food price +2"));
	negStatsText.insert(std::pair<std::string, std::string>("goodsPrice", "Goods price +2"));
	negStatsText.insert(std::pair<std::string, std::string>("techPrice", "Tech price +2"));
	negStatsText.insert(std::pair<std::string, std::string>("nuclearPrice", "Nuclear weapons \n20% more expensive"));
	negStatsText.insert(std::pair<std::string, std::string>("spacePrice", "Space program \n20% more expensive"));
	negStatsText.insert(std::pair<std::string, std::string>("spyPrice", "Spy network \n20% more expensive"));
	negStatsText.insert(std::pair<std::string, std::string>("popEatsMore", "Population eats \n10% more food"));

	std::map<std::string, float> posStatMap;
	posStatMap.insert(std::pair<std::string,float>("foodPrice", -3.f));
	posStatMap.insert(std::pair<std::string,float>("goodsPrice", -3.f));
	posStatMap.insert(std::pair<std::string,float>("techPrice", -3.f));
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
	negStatMap.insert(std::pair<std::string,float>("popEatsMore", 1.1f));

	int random = ( randomizer->randomNr(randomStats.size(),0) );

	mValues.insert(std::pair<std::string,float>(randomStats[random], posStatMap.find(randomStats[random])->second));
	mPositiveStats.push_back(posStatsText.find(randomStats[random])->second);
	randomStats[random] = randomStats.back();
	randomStats.pop_back();

	random = ( randomizer->randomNr(randomStats.size(),0) );
	mValues.insert(std::pair<std::string,float>(randomStats[random], posStatMap.find(randomStats[random])->second));
	mPositiveStats.push_back(posStatsText.find(randomStats[random])->second);
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
	mNegativeStats.push_back(negStatsText.find(randomStats[random])->second);
	randomStats[random] = randomStats.back();
	randomStats.pop_back();

	for(std::vector<std::string>::iterator it = randomStats.begin(); it != randomStats.end(); it++)
	{
		mValues.insert(std::pair<std::string, float>((*it), 0.f));
	}

	randomStats.clear();
}

void President::setName(std::string &string)
{
	mName = string;
}

std::string President::getName()const
{
	return mName;
}

void President::setFirstPositiveStat(std::string stat)
{
	mPositiveStats[0] = stat;
}

void President::setSecondPositiveStat(std::string stat)
{
	mPositiveStats[1] = stat;
}

void President::setNegativeStat(std::string stat)
{
	mNegativeStats[0] = stat;
}

std::string& President::getFirstPositiveStat()
{
	return mPositiveStats[0];
}
std::string& President::getSecondPositiveStat()
{
	return mPositiveStats[1];
}
std::string& President::getNegativeStat()
{
	return mNegativeStats[0];
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
	return mValues["popEatsMore"] == 0 ? 1 : mValues["popEatsMore"];
}

void President::setFoodPriceModifier(float value)
{
	mValues["foodPrice"] = value;
}

void President::setGoodsPriceModifier(float value)
{
	mValues["goodsPrice"] = value;
}

void President::setTechPriceModifier(float value)
{
	mValues["techPrice"] = value;
}

void President::setNuclearPriceModifier(float value)
{
	mValues["nuclearPrice"] = value;
}

void President::setSpacePriceModifier(float value)
{
	mValues["spacePrice"] = value;
}

void President::setSpyPriceModifier(float value)
{
	mValues["spyPrice"] = value;
}

void President::setPatriotismTaxModifier(float value)
{
	mValues["patriotismTax"] = value;
}

void President::setPopEatsMore(float value)
{
	mValues["popEatsMore"] = value;
}

void President::initializeImages(std::string &path)
{
	ResourceHandler* handler = ResourceHandler::getInstance();
	mTexture = &handler->getTexture(path);
	mPortrait.setTexture(*mTexture);
	mClickedPortraitTexture = &handler->getTexture("PickedLeaders/president-" + mName + "-picked");
}

void President::playSlogan()
{
	ResourceHandler::getInstance()->getMusic("Slogans/" + mName)->setVolume(50);
	ResourceHandler::getInstance()->getMusic("Slogans/" + mName)->play();
}

void President::presidentAnimation()
{
	float progress = mAnimationClock.getElapsedTime().asMilliseconds()/mTimeOut;
	progress = std::min<float>(progress, 1.0f);
	int frame = progress * (mFrames-1);
	mCurrentFrame = frame;
	std::cout <<"current frame: " << mCurrentFrame << std::endl;
}

void President::resetAnimation()
{
	/*if(resetTimer != nullptr)
		resetTimer->killTimer();
	resetTimer = Timer::setTimer([=]()
	{
		mCurrentFrame = 0;
	}, mTimeOut, 1);
	mAnimationClock.restart();*/
}

sf::Texture* President::getCurrentAnimationFrame()const
{
	return textures[mName][mCurrentFrame];
}

sf::Texture* President::getTexture()
{
	return mTexture;
}

sf::Texture* President::getClickedTexture()
{
	return mClickedPortraitTexture;
}

std::shared_ptr<sf::Music> President::getSlogan()
{
	return ResourceHandler::getInstance()->getMusic("Slogans/" + mName);
}