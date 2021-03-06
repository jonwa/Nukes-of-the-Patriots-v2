#include "UdpServer.h"
#include <iostream>
#include "Event.h"

sf::UdpServer::UdpServer(unsigned short port):mUdpSocket(),mUdpThread(nullptr)
{
	mUdpSocket.bind(port);
	mUdpSocket.setBlocking(true);
	mUdpThread = new sf::Thread(&UdpServer::tick, this);
	mUdpThread->launch();
}

void sf::UdpServer::sendData(sf::Packet packet, sf::IpAddress ipAddress, unsigned short port)
{
	mUdpSocket.send(packet, ipAddress, port);
}

void sf::UdpServer::tick()
{
	while(true)
	{
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet packet;
		sf::Socket::Status status = mUdpSocket.receive(packet, sender, port);
		if(status == sf::Socket::Done)
		{
			char eventName[1024];
			int charlen = 0;
			packet>>eventName>>charlen;
			sf::Packet _packet;
			void *data = const_cast<void*>(packet.getData());
			char *startPos = (char*)data;
			startPos += sizeof(char*) + charlen + sizeof(int);
			data = startPos;
			std::size_t size = packet.getDataSize() - charlen - sizeof(int);
			_packet.append(data, size);
			Event::triggerEvent(eventName, _packet);
		}
	}
}

void sf::UdpServer::triggerClientEvent(std::string eventName, sf::Packet packet, sf::IpAddress receivingAddress, unsigned short receivingPort)
{
	sf::Packet _packet;
	_packet<<eventName.c_str()<<eventName.length();
	_packet.append(packet.getData(), packet.getDataSize());
	sendData(_packet, receivingAddress, receivingPort);
}