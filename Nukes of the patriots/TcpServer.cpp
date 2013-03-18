#include "TcpServer.h"
#include <iostream>
#include "Event.h"

sf::TcpServer::TcpServer(unsigned short port):mTcpListener(),mClients(),mTcpThread(nullptr)
{
	mTcpListener.listen(port);
	mTcpListener.setBlocking(true);
	mSocketSelector.add(mTcpListener);
	mTcpThread = new sf::Thread(&TcpServer::tick, this);
	mTcpThread->launch();
}

void sf::TcpServer::tick()
{
	while(true)
	{
		if(mSocketSelector.wait())
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
						switch(status)
						{
							case(sf::Socket::Done):
								break;
							case(sf::Socket::Disconnected):
								packet<<client.getRemoteAddress().toString()<<client.getRemotePort();
								Event::triggerEvent("onPlayerDisconnected", packet);
								std::cout<<" has disconnected!"<<std::endl;
								break;
							default:
								break;
						}
					}
				}
			}
		}
	}
}

void sf::TcpServer::sendDataToClient(sf::Packet packet, std::vector<sf::TcpSocket*> clients)
{
	for(std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->send(packet);
	}
}