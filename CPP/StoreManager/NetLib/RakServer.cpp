#include <vector>
#include <thread>
#include "RakServer.h"

RakServer::RakServer()
	:m_bFinished(false)
{
	m_clientInfoList.clear();
	m_pServer = RakNet::RakPeerInterface::GetInstance();
}

RakServer::~RakServer()
{
	m_bFinished = true;
	m_pServer->Shutdown(50);

	RakNet::RakPeerInterface::DestroyInstance(m_pServer);

	m_pServer = nullptr;
}

bool RakServer::Init(const  RakNet::RakString& password, unsigned short port)
{
	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = port;
	socketDescriptor.socketFamily = AF_INET; // IPV4

	m_pServer->SetIncomingPassword(password.C_String(), password.GetLength());
	auto result = m_pServer->Startup((unsigned short)MAX_CLIENTS, &socketDescriptor, 1);
	bool success = result == RakNet::RAKNET_STARTED;
	RakAssert(success);

	m_pServer->SetMaximumIncomingConnections(MAX_CLIENTS);
	m_pServer->SetTimeoutTime(TIMEOUT_TIME, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	m_pServer->SetOccasionalPing(true);
	m_pServer->SetUnreliableTimeout(UNRELIABLE_TIMEOUT);

	m_pServer->AttachPlugin(&m_rpc);

	std::thread netThread(std::bind(&RakServer::UpdateNetwork, this));
	netThread.detach();

	return success;
}

void RakServer::UpdateNetwork()
{
	while (!m_bFinished)
	{
		if (m_pServer == nullptr) return;
		RakNet::Packet *p = m_pServer->Receive();
		while (p)
		{
			switch (GetPacketIdentifier(p))
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				OutputDebugStringA("Another client has disconnected");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				OutputDebugStringA("Another client has lost the connection");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				OutputDebugStringA("Another client has connected");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				OutputDebugStringA("Our connection request has been accepted\n");
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				//连接成功
				OutputDebugStringA("A connection is incoming\n");
			}
			break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				OutputDebugStringA("The server is full\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				OutputDebugStringA("A client has disconnected\n");
				//连接断开
				//ResetClientInfo(p->systemAddress);
				break;
			case ID_CONNECTION_LOST:
			{
				//连接断开
				//ResetClientInfo(p->systemAddress);
				OutputDebugStringA("A client lost the connection\n");
			}
			break;
			default:
				break;
			}

			m_pServer->DeallocatePacket(p);
			p = m_pServer->Receive();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	
}

int RakServer::Send(const char * data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast)
{
	return m_pServer->Send(data, length, priority, reliability, orderingChannel, systemIdentifier, broadcast);
}

int RakServer::Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast)
{
	return m_pServer->Send(bitStream, priority, reliability, orderingChannel, systemIdentifier, broadcast);
}

void RakServer::SendLoopback(const char * data, const int length)
{
	m_pServer->SendLoopback(data, length);
}


void RakServer::AttachPlugin(RakNet::PluginInterface2 *plugin)
{
	m_pServer->AttachPlugin(plugin);
}


unsigned char RakServer::GetPacketIdentifier(RakNet::Packet *p, int index /*= 0*/)
{
	if (p == 0) return 255;

	if ((unsigned char)p->data[index] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[index];
}

void RakServer::CloseConnection(const RakNet::AddressOrGUID target, bool sendDisconnectionNotification)
{
	m_pServer->CloseConnection(target, sendDisconnectionNotification);
}

void RakServer::RegisterSlot(const char * sharedIdentifier, void(*functionPointer)(RakNet::BitStream *userData, RakNet::Packet *packet), int callPriority)
{
	m_rpc.RegisterSlot(sharedIdentifier, functionPointer, callPriority);
}

bool RakServer::UnregisterSlot(const char* sharedIdentifier)
{
	return m_rpc.UnregisterSlot(sharedIdentifier);
}

void RakServer::Signal(const char * sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast, bool invokeLocal)
{
	m_rpc.Signal(sharedIdentifier, bitStream, priority, reliability, orderingChannel, systemIdentifier, broadcast, invokeLocal);
}

bool RakServer::RegisterBlockingFunction(const char * uniqueID, void(*functionPointer)(RakNet::BitStream *userData, RakNet::BitStream *returnData, RakNet::Packet *packet))
{
	return m_rpc.RegisterBlockingFunction(uniqueID, functionPointer);
}

bool RakServer::CallBlocking(const char * uniqueID, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, RakNet::BitStream * returnData)
{
	return m_rpc.CallBlocking(uniqueID, bitStream, priority, reliability, orderingChannel, systemIdentifier, returnData);
}
