#include "TcpClient.h"
#include <iostream>

sf::TcpClient::TcpClient(unsigned short serverPort, sf::IpAddress serverAddress):
	mTcpSocket(),mTcpThread(nullptr),mTcpConnect(nullptr),mServerPort(serverPort),mServerAddress(serverAddress)
{
	mTcpSocket.setBlocking(true);
	mTcpConnect = new sf::Thread(&TcpClient::connect, this);
	mTcpThread = new sf::Thread(&TcpClient::tick, this);
	mTcpConnect->launch();
	mTcpThread->launch();
}

void sf::TcpClient::connect()
{
	bool connected = false;
	while(!connected)
	{
		std::cout<<"attempting to connect..."<<std::endl;
		sf::Socket::Status status = mTcpSocket.connect(mServerAddress, mServerPort);
		switch(status)
		{
			case(sf::Socket::Done):
				connected = true;
				std::cout<<"You connected to the server!"<<std::endl;
				break;
			default:
				std::cout<<"Error code ("<<status<<")"<<std::endl;
		}
	}
}

void sf::TcpClient::tick()
{
	while(true)
	{
		sf::Packet packet;
		sf::Socket::Status status = mTcpSocket.receive(packet);
		switch(status)
		{
			case(sf::Socket::Done):
				break;
			default:
				break;
		}
	}
}

void sf::TcpClient::sendDataToServer(sf::Packet packet)
{
	if(mTcpSocket.getRemoteAddress() != sf::IpAddress::None)
		mTcpSocket.send(packet);
}