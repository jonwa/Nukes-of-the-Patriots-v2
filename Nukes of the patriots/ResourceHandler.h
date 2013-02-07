/*
 *	Denna klass tar hand om alla typer av resurshanteringar som anv�nds i spelet
 *	Nukes of the Patriots.
 *
 *	Dessa resurser �r inh�mtning av : 
 *	 
 *	 -Bilder
 *	 -Ljud
 *	 -Musik

 *		Av: Jon Wahlstr�m 2013-01-20
 */

#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Audio\Music.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\System\Clock.hpp>

#include <string>
#include <map>

class ResourceHandler
{
public:
	~ResourceHandler();
	
	//Singleton m�nster f�r instansiering
	static ResourceHandler* getInstance();

	//Texture funktioner
	void loadTexture(const std::string &path, const std::string &key);
	sf::Texture& getTexture(std::string &path);

	//Sound funktioner
	void loadSound(const std::string &path, const std::string &key);
	/*sf::SoundBuffer& getSound(std::string &path);*/

	//Music funktioner
	void loadMusic(const std::string &path, const std::string &key);
	std::shared_ptr<sf::Music> getMusic(std::string &path);

	void loadImages();
	void loadSounds();
	void load();

private:
	ResourceHandler();
	ResourceHandler operator = (const ResourceHandler& resourceHandler) { }
	ResourceHandler(const ResourceHandler& copy) { }

	//Singleton m�nster f�r instansiering
	static ResourceHandler* mInstance;

	//std::map :ar inneh�llandes Images, SoundBuffers and Music
	std::map<std::string, sf::Texture> Images;
	std::map<std::string, std::shared_ptr<sf::Music> > Music;
	std::map<std::string, sf::SoundBuffer> Sounds;
	
};
#endif