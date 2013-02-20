#pragma once
#include <string>

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include "Animation.h"


//		Simpel animationsklass, av Joakim Stark
//		Initiering av bilderna körs i konstruktorn
//		Användningsexempel:
//		mAnimation->updateImages();		sf::Sprite mSprite = mAnimation->getSprite();
//		
//		Finns även 3 hjälpfunktioner för att ändra position och lite skit.

class SpriteAnimation: public Animation
{
	public:
		SpriteAnimation(const std::string& filename, int timePerFrame, int frameCount);
		~SpriteAnimation(){};

		void initializeTextures(const std::string &path);
		void updateImages();

		void setPosition(const sf::Vector2f &position);
		const sf::Sprite &getSprite() const;
		bool endOfSpriteAnimation();
		void tick();
		
	private:
		int mFrameCount, mTimePerFrame, mCurrentFrame;  
		bool mEndOfSpriteAnimation;
		sf::Clock mFrameTimer;
		sf::Texture mTexture;
		sf::Sprite mSprite;

};