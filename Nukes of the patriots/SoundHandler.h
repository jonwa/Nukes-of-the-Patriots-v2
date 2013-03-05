#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H

#include <SFML\Audio\Music.hpp>
#include <memory>

class Sound;

class SoundHandler
{
public:
	static SoundHandler* getInstance();

	void addSound(std::shared_ptr<Sound> music);
	void tick();
	void reset();

private:
	SoundHandler();
	~SoundHandler();
	static SoundHandler* mInstance;
	std::vector<std::shared_ptr<Sound> > mMusicVector;

};

#endif