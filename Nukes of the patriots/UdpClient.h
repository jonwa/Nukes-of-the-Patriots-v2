#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H

#include <SFML\Network.hpp>
#include <SFML\System.hpp>
#include <string>

namespace sf
{
class UdpClient
{
public:
						UdpClient(unsigned short port, unsigned short serverPort, sf::IpAddress serverAddress);
	unsigned short		getPort()const{ return mUdpSocket.getLocalPort(); }
	sf::IpAddress		getReceivingAddress()const{ return mServerAddress; }

	void				setPort(unsigned short port){ mUdpSocket.bind(port); }
	void				setReceivingAddress(std::string serverAddress){ mServerAddress = sf::IpAddress::IpAddress(serverAddress); }
	void				tick();
	void				triggerServerEvent(std::string eventName, sf::Packet packet);
						~UdpClient(){delete mUdpThread;};
private:
	void				sendData(sf::Packet packet);
private:
	sf::UdpSocket mUdpSocket;
	unsigned short mServerPort;
	sf::IpAddress mServerAddress;
	sf::Thread *mUdpThread;
};
}


#endif