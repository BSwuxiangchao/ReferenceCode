
#include <string>
#include <vector>
#include <map>
#include <thread>
#include "RakClient.h"

RakClient::RakClient()
	:m_bFinished(false)
	,m_reconnectionInterval(500)
	,m_type(Video)
{
	m_lastConnectionTime = RakNet::GetTimeMS();
	m_pClient = RakNet::RakPeerInterface::GetInstance();
	m_lastLookupTime = RakNet::GetTimeMS();
	m_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}

RakClient::~RakClient()
{
	DisconnectFromServer();
	m_pClient->Shutdown(100);

	RakNet::RakPeerInterface::DestroyInstance(m_pClient);

	m_bFinished = true;
}

bool RakClient::Init(const char * clientName, const char *password, const char * serverIp, unsigned short serverPort)
{
	m_strClientName = clientName;
	m_strPassword = password;

	m_strServerIp = serverIp;
	m_iServerPort = serverPort;

	m_serverAddress.FromStringExplicitPort(serverIp, serverPort);

	m_pClient->AttachPlugin(&m_rpc);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = 0;
	socketDescriptor.socketFamily = AF_INET;
	RakNet::StartupResult b = m_pClient->Startup(1, &socketDescriptor, 1);
	
	m_pClient->SetOccasionalPing(true);
	//m_pClient->SetTimeoutTime(TIMEOUT_TIME, UNASSIGNED_SYSTEM_ADDRESS);
	m_pClient->SetUnreliableTimeout(UNRELIABLE_TIMEOUT);

	std::thread netThread(std::bind(&RakClient::UpdateNetwork, this));
	netThread.detach();

	return b == RakNet::RAKNET_STARTED;
}

bool RakClient::Init(const char*clientName, const char *password, unsigned short serverPort)
{
	m_strClientName = clientName;
	m_strPassword = password;

	m_strServerIp = "";
	m_iServerPort = serverPort;

	m_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	m_pClient->AttachPlugin(&m_rpc);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = 0;
	socketDescriptor.socketFamily = AF_INET;
	RakNet::StartupResult b = m_pClient->Startup(1, &socketDescriptor, 1);

	m_pClient->SetOccasionalPing(true);
	//m_pClient->SetTimeoutTime(TIMEOUT_TIME, UNASSIGNED_SYSTEM_ADDRESS);
	m_pClient->SetUnreliableTimeout(UNRELIABLE_TIMEOUT);

	std::thread netThread(std::bind(&RakClient::UpdateNetwork, this));
	netThread.detach();

	return b == RakNet::RAKNET_STARTED;
}

void RakClient::UpdateNetwork()
{
	while (!m_bFinished)
	{
		if (m_pClient == nullptr) return;

		RakNet::Packet *p = m_pClient->Receive();
		while (p)
		{
			switch (GetPacketIdentifier(p))
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				OutputDebugStringA("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				OutputDebugStringA("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				OutputDebugStringA("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				OutputDebugStringA("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				OutputDebugStringA("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				OutputDebugStringA("We are banned from this server.\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				//m_bConnected = false;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				break;

			case ID_INVALID_PASSWORD:
				OutputDebugStringA("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				OutputDebugStringA("ID_CONNECTION_LOST\n");
				break;
			case ID_UNCONNECTED_PONG:
			{
				m_strServerIp = p->systemAddress.ToString(false);
				m_serverAddress = p->systemAddress;

				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
			{
				OutputDebugStringA("connection request accepted, verifying...\n");
				// 向服务端发送客户端名称和密码，进行身份验证
				RakNet::BitStream bsOut;
				bsOut.Write((BYTE)ID_LOGIN_REQUEST);

				// 注意: 使用 WriteAlignedBytes 方法向服务端发送字符串,兼容C#客户端的中文字符问题
				int length = m_strClientName.GetLength();
				bsOut.Write(length);
				bsOut.WriteAlignedBytes((const unsigned char*)m_strClientName.C_String(), length);

				bsOut.WriteCompressed(m_strPassword);
				bsOut.Write((int)m_type);
				bsOut.Write((int)Cpp);

				m_pClient->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);

				//只要连上服务器，则把时间还原，防止下次断网后连接等待时间过长
				m_reconnectionInterval = 500;
			}
			break;
			default:
				break;

			}

			m_pClient->DeallocatePacket(p);
			p = m_pClient->Receive();
		}

		// 自动查找服务器
		LookupServerAutomatic();

		// 处理网络重连
		if (IsServerAddressAvailable())
		{
			ReconnectToServer();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void RakClient::ReconnectToServer()
{
	if (RakNet::GetTimeMS() - m_lastConnectionTime > m_reconnectionInterval)
	{
		RakNet::ConnectionState state = m_pClient->GetConnectionState(m_serverAddress);
		if (state == RakNet::ConnectionState::IS_NOT_CONNECTED || state == RakNet::ConnectionState::IS_DISCONNECTED)
		{
			ConnectToServer();
		}

		m_reconnectionInterval *= 2;
		if (m_reconnectionInterval >= 16000)
		{
			m_reconnectionInterval = 500;
		}

		m_lastConnectionTime = RakNet::GetTimeMS();
	}
}

bool RakClient::IsServerAddressAvailable()
{
	return m_serverAddress != RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}

int RakClient::Send(const char * data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast)
{
	return m_pClient->Send(data, length, priority, reliability, orderingChannel, systemIdentifier, broadcast);
}

int RakClient::Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast)
{
	return m_pClient->Send(bitStream, priority, reliability, orderingChannel, systemIdentifier, broadcast);
}

void RakClient::SendLoopback(const char * data, const int length)
{
	m_pClient->SendLoopback(data, length);
}

void RakClient::AttachPlugin(RakNet::PluginInterface2 *plugin)
{
	m_pClient->AttachPlugin(plugin);
}

void RakClient::ConnectToServer()
{
	if (m_pClient == nullptr || m_strServerIp.IsEmpty()) 
		return;

	if (m_pClient->Connect(m_strServerIp.C_String(), (unsigned short)m_iServerPort, m_strPassword.C_String(), m_strPassword.GetLength(), 0) == RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		OutputDebugStringA("Connecting to server...\n");
	}
	else
	{
		OutputDebugStringA("Connecting to server, error occurred.\n");
	}
}

void RakClient::DisconnectFromServer()
{
	if (m_pClient == nullptr) return;

	m_pClient->CloseConnection(m_serverAddress, true, 0);
}

unsigned char RakClient::GetPacketIdentifier(RakNet::Packet * p, int index)
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

void RakClient::LookupServerAutomatic()
{
	const int LOCAL_LOOKUP_TIME = 1000;
	if (m_pClient != nullptr && !IsServerAddressAvailable())
	{
		// 方便本机调试，优先查找本机运行的服务端程序
		if (RakNet::GetTimeMS() - m_lastLocalLookupTime > LOCAL_LOOKUP_TIME)
		{
			m_pClient->Ping(m_pClient->GetLocalIP(0), m_iServerPort, false);

			m_lastLocalLookupTime = RakNet::GetTimeMS();
		}

		if ((RakNet::GetTimeMS() - m_lastLookupTime > LOOKUP_INTERVAL))
		{
			m_pClient->Ping("255.255.255.255", m_iServerPort, false);

			m_lastLookupTime = RakNet::GetTimeMS();
		}
	}
}

bool RakClient::IsConnected()
{
	if (m_pClient == nullptr || !IsServerAddressAvailable())
		return false;

	return m_pClient->GetConnectionState(m_serverAddress) == RakNet::ConnectionState::IS_CONNECTED;
}

void RakClient::RegisterSlot(const char * sharedIdentifier, void(*functionPointer)(RakNet::BitStream *userData, RakNet::Packet *packet), int callPriority)
{
	m_rpc.RegisterSlot(sharedIdentifier, functionPointer, callPriority);
}

bool RakClient::UnregisterSlot(const char* sharedIdentifier)
{
	return m_rpc.UnregisterSlot(sharedIdentifier);
}

void RakClient::Signal(const char * sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast, bool invokeLocal)
{
	m_rpc.Signal(sharedIdentifier, bitStream, priority, reliability, orderingChannel, systemIdentifier, broadcast, invokeLocal);
}

bool RakClient::RegisterBlockingFunction(const char * uniqueID, void(*functionPointer)(RakNet::BitStream *userData, RakNet::BitStream *returnData, RakNet::Packet *packet))
{
	return m_rpc.RegisterBlockingFunction(uniqueID, functionPointer);
}

bool RakClient::CallBlocking(const char * uniqueID, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, RakNet::BitStream * returnData)
{
	return m_rpc.CallBlocking(uniqueID, bitStream, priority, reliability, orderingChannel, systemIdentifier, returnData);
}

