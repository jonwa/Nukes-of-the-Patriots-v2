#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include <SFML\Network.hpp>
#include <SFML\System.hpp>
#include <string>

namespace sf
{
class TcpClient
{
public:
						TcpClient(unsigned short serverPort, sf::IpAddress serverAddress);
	unsigned short		getServerPort()const{ return mServerPort; }
	sf::IpAddress		getServerAddress()const{ return mServerAddress; }
	void				tick();
	void				connect();
	void				sendDataToServer(sf::Packet packet);
						~TcpClient(){delete mTcpThread; delete mTcpConnect;};
private:
	sf::TcpSocket mTcpSocket;
	unsigned short mServerPort;
	sf::IpAddress mServerAddress;
	sf::Thread *mTcpThread, *mTcpConnect;
};
}


#endif