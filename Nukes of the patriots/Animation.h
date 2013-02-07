#pragma once
#include <string>

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>


//		Simpel animationsklass, av Joakim Stark
//		Initiering av bilderna körs i konstruktorn
//		Användningsexempel:
//		mAnimation->updateImages();		sf::Sprite mSprite = mAnimation->getSprite();
//		
//		Finns även 3 hjälpfunktioner för att ändra position och lite skit.

class Animation
{
	public:
		Animation(const std::string& filename, int timePerFrame, int frameCount);
		~Animation();

		void initializeTextures(const std::string &path);
		void updateImages();

		void setPosition(const sf::Vector2f &position);
		const sf::Sprite &getSprite() const;
		bool endOfAnimation();
		
		
	private:
		int mFrameCount, mTimePerFrame, mCurrentFrame;  
		bool mEndOfAnimation;
		sf::Clock mFrameTimer;
		sf::Texture mTexture;
		sf::Sprite mSprite;

};