#include "TcpClient.h"
#include <iostream>
#include "Event.h"

sf::TcpClient::TcpClient(unsigned short serverPort, sf::IpAddress serverAddress):
	mTcpSocket(),mTcpThread(nullptr),mTcpConnect(nullptr),mServerPort(serverPort),mServerAddress(serverAddress)
{
	mTcpSocket.setBlocking(true);
	mTcpConnect = new sf::Thread(&TcpClient::connect, this);
	//mTcpThread = new sf::Thread(&TcpClient::tick, this);
	mTcpConnect->launch();
	//mTcpThread->launch();
}

void sf::TcpClient::connect()
{
	bool connected = false;
	while(!connected)
	{
		mTcpSocket.setBlocking(true);
		sf::Socket::Status status = mTcpSocket.connect(mServerAddress, mServerPort);
		if(status == sf::Socket::Done)
		{
			connected = true;
			sf::Packet packet;
			packet<<mServerAddress.toString()<<mServerPort;
			Event::triggerEvent("onClientPlayerConnected", packet);
			//std::cout<<"You connected to the server!"<<std::endl;
		}
	}
}

void sf::TcpClient::tick()
{
	//while(true)
	//{
		sf::Packet packet;
		mTcpSocket.setBlocking(false);
		sf::Socket::Status status = mTcpSocket.receive(packet);
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
		else if(status == sf::Socket::Disconnected)
		{
			std::cout << "server died!" << std::endl;
		}
	//}
}

void sf::TcpClient::sendDataToServer(sf::Packet packet)
{
	if(mTcpSocket.getRemoteAddress() != sf::IpAddress::None)
		mTcpSocket.send(packet);
}

void sf::TcpClient::triggerServerEvent(std::string eventName, sf::Packet packet)
{
	sf::Packet _packet;
	_packet<<eventName.c_str()<<eventName.length();
	_packet.append(packet.getData(), packet.getDataSize());
	mTcpSocket.send(_packet);
}