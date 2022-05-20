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

	/// 向特定的网络连接对象发送数据
	/// 注意：这个函数只在网络已经连接上才会起作用
	/// 注意：数据的第一个字节应该是一个以 ID_USER_PACKET_ENUM 开始的消息标志
	/// data： 发送的数据.
	/// length： 数据长度，单位字节.
	/// priority：发送的优先级. 具体请查看 PacketPriority.h
	/// reliability： 数据发送的可靠性，具体请查看 PacketPriority.h
	/// orderingChannel：当使用有序（ordered）或者 sequenced 消息时, 发送消息的通道. 消息只相对于同一流中的其他消息排序（ordered）
	/// systemIdentifier：向哪个对象发送数据包，或者在广播的情况下不向哪个对象发送数据包. 传递一个 SystemAddress 或者一个 RakNetGUID 结构体. 使用 UNASSIGNED_SYSTEM_ADDRESS 的时候不指定对象
	/// broadcast：为 True 时向所有连接的对象发送. 如果为 true, 那么 systemAddress 就是排除的发送对象.
	/// 返回值：返回 0 表示错误输入. 其他返回发送成功的消息字节数. 
	int Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	int Send(const RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const RakNet::AddressOrGUID systemIdentifier, bool broadcast);
	void SendLoopback(const char *data, const int length);
	void AttachPlugin(RakNet::PluginInterface2 *plugin);
	void CloseConnection(const RakNet::AddressOrGUID target, bool sendDisconnectionNotification);

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
	void UpdateNetwork();
	unsigned char GetPacketIdentifier(RakNet::Packet *p, int index = 0);
	
private:
	RakNet::RakPeerInterface*	 m_pServer;
	std::vector<ClientInfo>		 m_clientInfoList;
	bool						 m_bFinished;
	RakNet::RPC4				 m_rpc;
};



#endif