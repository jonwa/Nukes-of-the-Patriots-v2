#include "Sound.h"
#include "SoundHandler.h"
#include <iostream>

std::shared_ptr<Sound> Sound::create(std::shared_ptr<sf::Music> music)
{
	std::shared_ptr<Sound> result = std::make_shared<Sound>(music);
	SoundHandler::getInstance()->addSound(result);
	return result;
}

Sound::Sound(std::shared_ptr<sf::Music> music) :
	mMusic(music),
	mTimer(nullptr),
	mTime(0),
	mStartVolume(100),
	mEndVolume(100)
{
	if(mMusic != nullptr)
	{
		mStartVolume = mMusic->getVolume();
		mEndVolume = mMusic->getVolume();
	}
}

Sound::~Sound(){}

void Sound::fadeToVolume(int time, int startVolume, int endVolume)
{
	if(mTimer != nullptr)
		mTimer->killTimer();
	mTimer = Timer::setTimer([=](){}, time, 1);
	mStartVolume = startVolume;
	mEndVolume = endVolume;
}

void Sound::tick()
{
	if(mMusic != nullptr)
	{
		float timeleft = 0;
		float duration = 1;
		if(Timer::isTimer(mTimer) && mTimer != NULL)
		{
			timeleft = mTimer->getTimeLeft();
			duration = mTimer->getTimerDuration();
		}
		float progress = 1 - (timeleft/duration);
		int volumeDiff = mEndVolume - mStartVolume;
		mMusic->setVolume(mStartVolume + volumeDiff*progress);
	}
}


void  Sound::playSound(bool loop)
{
	mMusic->play();
	mMusic->setLoop(loop);
}

void Sound::stopSound()
{
	mMusic->stop();
}

void Sound::pauseSound()
{
	mMusic->pause();
}

void Sound::setVolume(int volume)
{
	mMusic->setVolume(volume);
	mStartVolume = volume;
	mEndVolume = volume;
}

void Sound::setSound(std::shared_ptr<sf::Music> sound)
{
	mMusic = sound;
	mMusic->setVolume(sound->getVolume());
}

int Sound::getVolume()
{
	return mMusic->getVolume();
}

std::shared_ptr<sf::Music> Sound::getSound()
{
	return mMusic;
}


