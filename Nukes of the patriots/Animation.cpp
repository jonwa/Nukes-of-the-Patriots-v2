#include "Animation.h"


/* Konstruktor, tar argument för filnamn, tid per bildruta och antal bildrutor(i spriten)  */
Animation::Animation(const std::string& filename, int timePerFrame, int frameCount):
	mTimePerFrame(timePerFrame),
		mFrameCount(frameCount),
		mCurrentFrame(0),
		mEndOfAnimation(false)

{
	initializeTextures(filename);
}



void Animation::initializeTextures(const std::string &path)
{
	mTexture.loadFromFile(path);
	mSprite.setTexture(mTexture);
	sf::IntRect textureRect(0,0,mTexture.getSize().x, mTexture.getSize().y/mFrameCount);
	mSprite.setTextureRect(textureRect);
	
}



void Animation::updateImages()
{
	if(mFrameTimer.getElapsedTime().asMilliseconds() > mTimePerFrame)
	{
		mFrameTimer.restart();
		++mCurrentFrame;

		if(mCurrentFrame >= mFrameCount)
		{
			mEndOfAnimation = true;
			mCurrentFrame = 0;
		}

		else
		{
			mEndOfAnimation = false;
		}

		sf::IntRect currentRect = mSprite.getTextureRect();
		currentRect.top = currentRect.height * mCurrentFrame;
		mSprite.setTextureRect(currentRect);
	}

}



/* Hjälpfunktioner   */

/* Bestäm spritens position */
void Animation::setPosition(const sf::Vector2f &position)
{
	mSprite.setPosition(position);
}

/* Hämtar objektets sprite */
const sf::Sprite &Animation::getSprite() const
{
	return mSprite;
}

/* Kollar om en animation har nått sitt slut */
bool Animation::endOfAnimation()
{
	return mEndOfAnimation;
}



