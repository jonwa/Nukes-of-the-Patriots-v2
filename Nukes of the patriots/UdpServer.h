#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include <SFML\Network.hpp>
#include <SFML\System.hpp>
#include <vector>
#include <string>

namespace sf
{
class UdpServer
{
public:
						UdpServer(unsigned short port);
	unsigned short		getPort()const{ return mUdpSocket.getLocalPort(); }

	void				setPort(unsigned short port){ mUdpSocket.bind(port); }
	void				tick();
	void				triggerClientEvent(std::string eventName, sf::Packet packet, sf::IpAddress receivingAddress, unsigned short receivingPort);
						~UdpServer(){delete mUdpThread;};
private:
	void				sendData(sf::Packet packet, sf::IpAddress ipAddress, unsigned short port);
private:
	sf::UdpSocket mUdpSocket;
	sf::Thread *mUdpThread;
};
};

#endif