#include "SoundHandler.h"
#include "Sound.h"

SoundHandler* SoundHandler::mInstance = NULL;
SoundHandler* SoundHandler::getInstance()
{
	if(mInstance == NULL)
		mInstance = new SoundHandler();
	return mInstance;
}

SoundHandler::SoundHandler(){}
SoundHandler::~SoundHandler(){}


void SoundHandler::tick()
{ 
	for(std::vector<std::shared_ptr<Sound> >::size_type it = 0; it < mMusicVector.size(); it++)
	{
		mMusicVector[it]->tick();
	}
}

void SoundHandler::reset()
{
	for(std::vector< std::shared_ptr<Sound> >::size_type it = 0; it < mMusicVector.size(); it++)
	{
		mMusicVector[it].reset();
	}
	mMusicVector.clear();
}

void SoundHandler::addSound(std::shared_ptr<Sound> music)
{
	mMusicVector.push_back(music);
}
