#ifndef SOUND_H
#define SOUND_H

#include "Timer.h"
#include <SFML/Audio.hpp>



class Sound
{
public:
	Sound(std::shared_ptr<sf::Music> music);
	~Sound();

	static std::shared_ptr<Sound> create(std::shared_ptr<sf::Music> music);
	void fadeToVolume(std::shared_ptr<sf::Music> music, int time, int startVolume, int endVolume);
	void tick();

	void playSound(bool loop = false);
	void stopSound();
	void pauseSound();
	void setVolume(int volume);
	
private:
	Timer* mTimer;
	std::shared_ptr<sf::Music> mMusic;
	int mTime, mStartVolume, mEndVolume;

};
#endif