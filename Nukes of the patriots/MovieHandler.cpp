#include "MovieHandler.h"
#include <SFML\Graphics\RectangleShape.hpp>

MovieHandler* MovieHandler::mInstance = NULL;

MovieHandler* MovieHandler::getInstance()
{
	if(mInstance == nullptr)
		mInstance = new MovieHandler();

	return mInstance;
}

MovieHandler::MovieHandler() : 
	mMovie(nullptr),
	mLoaded(true),
	mThread(nullptr)
{
}


MovieHandler::~MovieHandler()
{
}

void MovieHandler::init(sf::RenderWindow &window)
{
	mWindow = &window;
	mThread = new sf::Thread([=]()
	{
		mMovie->play();
		int timesPlayed = 0;
		mWindow->setActive(false);
		while(!isLoaded() || (mMovie->getStatus() == sfe::Movie::Playing && timesPlayed == 0))
		{
			mWindow->setActive(true);
			mWindow->clear();
			mWindow->draw(*mMovie);
			//sf::RectangleShape rect(sf::Vector2f(200, 50));
			//rect.setFillColor(sf::Color(0, 255, 0, 255));
			//mWindow->draw(rect);
			mWindow->display();
			if(mMovie->getStatus() == sfe::Movie::Stopped)
			{
				timesPlayed++;
				mMovie->play();
			}
		}
	});
}

sfe::Movie* MovieHandler::getMovie()
{
	return mMovie;
}

bool MovieHandler::isLoaded()
{
	return mLoaded;
}

void MovieHandler::setLoaded(bool loaded)
{
	mLoaded = loaded;
}

void MovieHandler::setMovie(sfe::Movie &movie)
{
	mMovie = &movie;
}

void MovieHandler::playMovie()
{
	mWindow->setActive(false);
	mThread->launch();
}

void MovieHandler::drawMovie()
{
	mWindow->draw(*mMovie);
}