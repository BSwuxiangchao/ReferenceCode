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

	// 网络初始化
	// clientName : 客户端名称，要匹配服务端配置的客户端名称列表中的一项
	// password: 网络连接密码
	// serverIp : 服务端ip地址
	// serverPort : 服务端网络端口
	bool Init(const char*clientName, const char *password, const char* serverIp, unsigned short serverPort);
	bool Init(const char*clientName, const char *password, unsigned short serverPort);

	int Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	int Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	void SendLoopback(const char *data, const int length);
	void AttachPlugin(RakNet::PluginInterface2 *plugin);

	//判断网络连接状态
	bool IsConnected();

	// 注册RPC函数，函数是非阻塞的，函数只能是C语言格式
	void RegisterSlot(const char *sharedIdentifier, void(*functionPointer) (RakNet::BitStream *userData, RakNet::Packet *packet), int callPriority);

	bool UnregisterSlot(const char* sharedIdentifier);

	// 调用RPC函数，调用之后立即返回，不等待函数执行结果
	void Signal(const char *sharedIdentifier, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast, bool invokeLocal);

	// 注册RPC函数，函数是阻塞的，函数只能是C语言格式
	bool RegisterBlockingFunction(const char* uniqueID, void(*functionPointer) (RakNet::BitStream *userData, RakNet::BitStream *returnData, RakNet::Packet *packet));

	// 调用RPC函数，调用之后等待函数执行结果才会返回
	bool CallBlocking(const char* uniqueID, RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, RakNet::BitStream *returnData);


private:
	void ConnectToServer();

	void ReconnectToServer();

	void DisconnectFromServer();
	
	void UpdateNetwork();

	void LookupServerAutomatic();

	bool IsServerAddressAvailable();

	RakNet::SystemAddress& GetServerAddress() { return m_serverAddress; }

	// 设置客户端类型，用于服务端的归类
	void SetType(enum ClientType type) { m_type = type; }

	unsigned char GetPacketIdentifier(RakNet::Packet *p, int index = 0);
private:
	RakNet::RakPeerInterface	*m_pClient;
	RakNet::RakString			m_strClientName;
	RakNet::RakString			m_strServerIp;
	int					m_iServerPort;
	RakNet::SystemAddress		m_serverAddress;
	RakNet::TimeMS		m_lastConnectionTime;
	RakNet::TimeMS		m_reconnectionInterval; // 网络重连时间间隔，为了网络连接效率，间隔时间采用指数避让的方式，分别是：500ms， 1000ms，2000ms，4000ms，8000ms，16000ms
	RakNet::TimeMS		m_lastLookupTime;
	RakNet::TimeMS		m_lastLocalLookupTime; // 为了方便本地调试，优先查找本地的总控服务器是否启动
	RakNet::RPC4		m_rpc;
	
	bool				m_bFinished;
	RakNet::RakString	m_strPassword;
	ClientType			m_type;
};




#endif