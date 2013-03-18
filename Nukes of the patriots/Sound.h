#ifndef SOUND_H
#define SOUND_H

#include "Timer.h"
#include <SFML/Audio.hpp>



class Sound
{
public:
	Sound(std::shared_ptr<sf::Music> music = 0);
	~Sound();

	static std::shared_ptr<Sound> create(std::shared_ptr<sf::Music> music = 0);
	void fadeToVolume(int time, int startVolume, int endVolume);
	void tick();

	void playSound(bool loop = false);
	void stopSound();
	void pauseSound();
	void setVolume(int volume);
	void setSound(std::shared_ptr<sf::Music> sound);
	int getVolume();
	std::shared_ptr<sf::Music> getSound();
private:
	Timer* mTimer;
	std::shared_ptr<sf::Music> mMusic;
	int mTime, mStartVolume, mEndVolume;
};
#endif