#pragma once
#include <string>
#include "randomizer.h"
#include <vector>
#include <map>

#include <SFML/Graphics.hpp>

// President-klass av Joakim Stark
//randomStatFunc har en vektor med random stats, den slumpar fram 3 stats som sedan får värden mappade till sig.
//
//Användningsexempel:
//		President *mPresident;
//		mPresident = new President(filename);
//		mFood = mPresident->getFoodPriceModifier();

class President
{
public:
	President(std::string& filename, std::string &name);
	~President();

	void initializeImages(std::string &path);
	sf::Texture* getTexture();
	sf::Texture* getClickedTexture();

	float getFoodPriceModifier();
	float getGoodsPriceModifier();
	float getTechPriceModifier();
	float getNuclearPriceModifier();
	float getSpacePriceModifier();
	float getSpyPriceModifier();
	float getPatriotismTaxModifier();
	float getPopEatsMore();

	int getYearsElected() { return mYearsElected; }
	void setYearsElected(int years) { mYearsElected = years; }
	void playSlogan();

	std::string& getFirstPositiveStat();
	std::string& getSecondPositiveStat();
	std::string& getNegativeStat();
	std::string& getBiography();
private:
	std::string mName;
	void randomStatFunc();
	int mYearsElected;
	sf::Sprite mPortrait;
	sf::Texture* mClickedPortraitTexture;
	sf::Texture* mTexture;
	std::map <std::string, float> mValues;
	std::vector<std::string> mPositiveStats;
	std::vector<std::string> mNegativeStats;
	std::vector<std::string> randomStats;
	
};