#ifndef _MOVIE_HANDLER_H
#define _MOVIE_HANDLER_H

#include <sfeMovie\Movie.hpp>
#include <SFML\Graphics\RenderWindow.hpp>

class MovieHandler
{
public:
	static MovieHandler* getInstance();
	~MovieHandler();

	void init(sf::RenderWindow &window);
	sfe::Movie *getMovie();
	bool isLoaded();

	void setLoaded(bool loaded);
	void setMovie(sfe::Movie &movie);
	void playMovie();
	void drawMovie();
private:
	static MovieHandler *mInstance;
	MovieHandler();
	MovieHandler(const MovieHandler&){}
	MovieHandler operator=(const MovieHandler&){}

	sf::Thread *mThread;
	sf::RenderWindow *mWindow;
	sfe::Movie *mMovie;
	bool mLoaded;
};

#endif