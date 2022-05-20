#pragma once

#ifndef RakServer_H__
#define RakServer_H__

#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GetTime.h"
#include "RPC4Plugin.h"
#include "Export.h"
#include "Common.h"
#include "RPCDefines.h"

class RAK_DLL_EXPORT RakServer
{
public:
	RakServer();
	virtual ~RakServer();

	bool Init(const RakNet::RakString& password, unsigned short port);

	/// ���ض����������Ӷ���������
	/// ע�⣺�������ֻ�������Ѿ������ϲŻ�������
	/// ע�⣺���ݵĵ�һ���ֽ�Ӧ����һ���� ID_USER_PACKET_ENUM ��ʼ����Ϣ��־
	/// data�� ���͵�����.
	/// length�� ���ݳ��ȣ���λ�ֽ�.
	/// priority�����͵����ȼ�. ������鿴 PacketPriority.h
	/// reliability�� ���ݷ��͵Ŀɿ��ԣ�������鿴 PacketPriority.h
	/// orderingChannel����ʹ������ordered������ sequenced ��Ϣʱ, ������Ϣ��ͨ��. ��Ϣֻ�����ͬһ���е�������Ϣ����ordered��
	/// systemIdentifier�����ĸ����������ݰ��������ڹ㲥������²����ĸ����������ݰ�. ����һ�� SystemAddress ����һ�� RakNetGUID �ṹ��. ʹ�� UNASSIGNED_SYSTEM_ADDRESS ��ʱ��ָ������
	/// broadcast��Ϊ True ʱ���������ӵĶ�����. ���Ϊ true, ��ô systemAddress �����ų��ķ��Ͷ���.
	/// ����ֵ������ 0 ��ʾ��������. �������ط��ͳɹ�����Ϣ�ֽ���. 
	int Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	int Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	void SendLoopback(const char *data, const int length);
	void AttachPlugin(RakNet::PluginInterface2 *plugin);
	void CloseConnection(const RakNet::AddressOrGUID target, bool sendDisconnectionNotification);

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
	void UpdateNetwork();
	unsigned char GetPacketIdentifier(RakNet::Packet *p, int index = 0);
	
private:
	RakNet::RakPeerInterface*	 m_pServer;
	std::vector<ClientInfo>		 m_clientInfoList;
	bool						 m_bFinished;
	RakNet::RPC4				 m_rpc;
};



#endif