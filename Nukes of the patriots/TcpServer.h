#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <SFML\Network.hpp>
#include <SFML\System.hpp>
#include <vector>
#include <string>

namespace sf
{
class TcpServer
{
public:
						TcpServer(unsigned short port);
	unsigned short		getPort()const{ return mTcpListener.getLocalPort(); }

	void				setPort(unsigned short port){ mTcpListener.listen(port); }
	void				tick();
	void				sendDataToClient(sf::Packet packet, std::vector<sf::TcpSocket*> clients);
						~TcpServer(){delete mTcpThread;};
private:
	sf::TcpListener mTcpListener;
	sf::Thread *mTcpThread;

	std::vector<sf::TcpSocket*> mClients;
	sf::SocketSelector mSocketSelector;
};
};

#endif