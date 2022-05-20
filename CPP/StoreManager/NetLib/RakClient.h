#pragma once

#ifndef RakClient_H__
#define RakClient_H__

#include "Export.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GetTime.h"
#include "RPC4Plugin.h"
#include "Common.h"
#include "RPCDefines.h"

class RAK_DLL_EXPORT RakClient
{
public:
	RakClient();
	virtual ~RakClient();

	// �����ʼ��
	// clientName : �ͻ������ƣ�Ҫƥ���������õĿͻ��������б��е�һ��
	// password: ������������
	// serverIp : �����ip��ַ
	// serverPort : ���������˿�
	bool Init(const char*clientName, const char *password, const char* serverIp, unsigned short serverPort);
	bool Init(const char*clientName, const char *password, unsigned short serverPort);

	int Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	int Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	void SendLoopback(const char *data, const int length);
	void AttachPlugin(RakNet::PluginInterface2 *plugin);

	//�ж���������״̬
	bool IsConnected();

	// ע��RPC�����������Ƿ������ģ�����ֻ����C���Ը�ʽ
	void RegisterSlot(const char *sharedIdentifier, void(*functionPointer) (RakNet::BitStream *userData, RakNet::Packet *packet), int callPriority);

	bool UnregisterSlot(const char* sharedIdentifier);

	// ����RPC����������֮���������أ����ȴ�����ִ�н��
	void Signal(const char *sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast, bool invokeLocal);

	// ע��RPC�����������������ģ�����ֻ����C���Ը�ʽ
	bool RegisterBlockingFunction(const char* uniqueID, void(*functionPointer) (RakNet::BitStream *userData, RakNet::BitStream *returnData, RakNet::Packet *packet));

	// ����RPC����������֮��ȴ�����ִ�н���Ż᷵��
	bool CallBlocking(const char* uniqueID, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, RakNet::BitStream *returnData);


private:
	void ConnectToServer();

	void ReconnectToServer();

	void DisconnectFromServer();
	
	void UpdateNetwork();

	void LookupServerAutomatic();

	bool IsServerAddressAvailable();

	RakNet::SystemAddress& GetServerAddress() { return m_serverAddress; }

	// ���ÿͻ������ͣ����ڷ���˵Ĺ���
	void SetType(enum ClientType type) { m_type = type; }

	unsigned char GetPacketIdentifier(RakNet::Packet *p, int index = 0);
private:
	RakNet::RakPeerInterface	*m_pClient;
	RakNet::RakString			m_strClientName;
	RakNet::RakString			m_strServerIp;
	int					m_iServerPort;
	RakNet::SystemAddress		m_serverAddress;
	RakNet::TimeMS		m_lastConnectionTime;
	RakNet::TimeMS		m_reconnectionInterval; // ��������ʱ������Ϊ����������Ч�ʣ����ʱ�����ָ�����õķ�ʽ���ֱ��ǣ�500ms�� 1000ms��2000ms��4000ms��8000ms��16000ms
	RakNet::TimeMS		m_lastLookupTime;
	RakNet::TimeMS		m_lastLocalLookupTime; // Ϊ�˷��㱾�ص��ԣ����Ȳ��ұ��ص��ܿط������Ƿ�����
	RakNet::RPC4		m_rpc;
	
	bool				m_bFinished;
	RakNet::RakString	m_strPassword;
	ClientType			m_type;
};




#endif