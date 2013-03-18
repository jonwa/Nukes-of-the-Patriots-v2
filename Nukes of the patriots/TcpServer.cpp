#include "TcpServer.h"
#include <iostream>
#include "Event.h"

sf::TcpServer::TcpServer(unsigned short port):mTcpListener(),mClients(),mTcpThread(nullptr)
{
	mTcpListener.listen(port);
	mTcpListener.setBlocking(false);
	mSocketSelector.add(mTcpListener);
	//mTcpThread = new sf::Thread(&TcpServer::tick, this);
	//mTcpThread->launch();
}

void sf::TcpServer::tick()
{
	//while(true)
	//{
		if(mSocketSelector.wait(sf::seconds(0.001f)))
		{
			if(mSocketSelector.isReady(mTcpListener))
			{
				// listener is ready; pending connecting with a new client
				sf::TcpSocket* client = new sf::TcpSocket;
				sf::Socket::Status status = mTcpListener.accept(*client);
				if(status == sf::Socket::Done)
				{
					sf::Packet packet;
					packet<<client->getRemoteAddress().toString()<<client->getRemotePort();
					Event::triggerEvent("onPlayerConnected", packet);
					// add new client to client list
					mClients.push_back(client);
					// add the new client to the selector so we
					// will be notified when he sends something
					mSocketSelector.add(*client);
				}
			}
			else
			{
				// listener is not ready, test the other clients
				for(std::vector<sf::TcpSocket*>::iterator it = mClients.begin(); it != mClients.end(); ++it)
				{
					sf::TcpSocket& client = **it;
					if(mSocketSelector.isReady(client))
					{
						// The client has sent some data, time to get it!
						sf::Packet packet;
						sf::Socket::Status status = client.receive(packet);
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
							sf::Packet _packet;
							_packet<<client.getRemoteAddress().toString()<<client.getRemotePort();
							Event::triggerEvent("onPlayerDisconnected", _packet);
							std::cout<<" has disconnected!"<<std::endl;
						}
					}
				}
			}
		}
//	}
}

void sf::TcpServer::sendDataToClient(sf::Packet packet, std::vector<sf::TcpSocket*> clients)
{
	for(std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->send(packet);
	}
}

void sf::TcpServer::triggerClientEvent(std::string eventName, sf::Packet packet, sf::IpAddress receivingAddress, unsigned short receivingPort)
{
	sf::Packet _packet;
	_packet<<eventName.c_str()<<eventName.length();
	_packet.append(packet.getData(), packet.getDataSize());
	for(std::vector<sf::TcpSocket*>::iterator it = mClients.begin(); it != mClients.end(); ++it)
	{
		(*it)->send(_packet);
	}
}