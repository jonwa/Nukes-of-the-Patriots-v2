#include "SpriteAnimation.h"


/* Konstruktor, tar argument för filnamn, tid per bildruta och antal bildrutor(i spriten)  */
SpriteAnimation::SpriteAnimation(const std::string& filename, int timePerFrame, int frameCount):
	Animation(),
	mTimePerFrame(timePerFrame),
		mFrameCount(frameCount),
		mCurrentFrame(0),
		mEndOfSpriteAnimation(false)

{
	initializeTextures(filename);
}



void SpriteAnimation::initializeTextures(const std::string &path)
{
	mTexture.loadFromFile(path);
	mSprite.setTexture(mTexture);
	sf::IntRect textureRect(0,0,mTexture.getSize().x, mTexture.getSize().y/mFrameCount);
	mSprite.setTextureRect(textureRect);
	
}



void SpriteAnimation::updateImages()
{
	if(mFrameTimer.getElapsedTime().asMilliseconds() > mTimePerFrame)
	{
		mFrameTimer.restart();
		++mCurrentFrame;

		if(mCurrentFrame >= mFrameCount)
		{
			mEndOfSpriteAnimation = true;
			mCurrentFrame = 0;
		}

		else
		{
			mEndOfSpriteAnimation = false;
		}

		sf::IntRect currentRect = mSprite.getTextureRect();
		currentRect.top = currentRect.height * mCurrentFrame;
		mSprite.setTextureRect(currentRect);
	}

}



/* Hjälpfunktioner   */

/* Bestäm spritens position */
void SpriteAnimation::setPosition(const sf::Vector2f &position)
{
	mSprite.setPosition(position);
}

/* Hämtar objektets sprite */
const sf::Sprite &SpriteAnimation::getSprite() const
{
	return mSprite;
}

/* Kollar om en animation har nått sitt slut */
bool SpriteAnimation::endOfSpriteAnimation()
{
	return mEndOfSpriteAnimation;
}

void SpriteAnimation::tick()
{

}



