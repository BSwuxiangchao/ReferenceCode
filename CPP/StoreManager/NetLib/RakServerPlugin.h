#pragma once

#ifndef RAKSERVERPLUGIN_H__
#define RAKSERVERPLUGIN_H__

#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GetTime.h"
#include "RPC4Plugin.h"
#include "Common.h"
#include "Export.h"
#include "RPCDefines.h"

class RAK_DLL_EXPORT RakServerPlugin : public RakNet::PluginInterface2
{
public:
	virtual RakNet::PluginReceiveResult OnReceive(RakNet::Packet *packet) override;
	virtual void OnVerifyConnection(const RakNet::RakString &clientName, bool passed) { (void)clientName; (void)passed; }

	/// Called when a connection is dropped because the user called RakPeer::CloseConnection() for a particular system
	/// \param[in] systemAddress The system whose connection was closed
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] lostConnectionReason How the connection was closed: manually, connection lost, or notification of disconnection
	virtual void OnClosedConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID, RakNet::PI2_LostConnectionReason lostConnectionReason) override;

	/// Called when we got a new connection
	/// \param[in] systemAddress Address of the new connection
	/// \param[in] rakNetGuid The guid of the specified system
	/// \param[in] isIncoming If true, this is ID_NEW_INCOMING_CONNECTION, or the equivalent
	virtual void OnNewConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID, bool isIncoming) override { (void)systemAddress; (void)rakNetGUID; (void)isIncoming; }

	/// 处理客户端(主要是C#客户端, C++客户端采用RPC调用)发送过来的控制指令
	virtual void OnNetCommandRequest(RakNet::BitStream *bitStream) { }

	/// 处理客户端(主要是C#客户端, C++客户端采用RPC调用)返回的指令执行结果
	virtual void OnNetCommandResponse(RakNet::BitStream *bitStream) { }

	/// 处理客户端(主要是C#客户端, C++客户端采用RPC调用)返回的异常或者错误信息
	virtual void OnReceiveClientError(RakNet::BitStream *bitStream) { }

	virtual void OnReceivePLCStatus(RakNet::BitStream *bitStream) { }

	/// 处理客户端转发控制指令
	/// sourceClientName: 控制指令的来源客户端
	/// targetClientName: 控制指令的接收客户端
	/// request: 控制指令
	/// success: 是否转发成功
	virtual void OnForwardCommandRequest(const RakNet::RakString &sourceClientName, const RakNet::RakString &targetClientName, const RakNet::RakString &request, bool success) { }

	/// 处理客户端转发的控制指令执行结果
	/// sourceClientName: 执行结果的来源客户端
	/// targetClientName: 执行结果的接收客户端
	/// response: 执行指令/结果
	/// success: 是否执行成功
	virtual void OnForwardCommandResponse(const RakNet::RakString &sourceClientName, const RakNet::RakString &targetClientName, const RakNet::RakString &response, bool success) { }

	void SetClientInfoList(const std::vector<ClientInfo> &clientInfoList) { m_clientInfoList = clientInfoList; }
	std::vector<ClientInfo> & GetClientInfoList() { return m_clientInfoList; }
	std::vector<RakNet::RakString> & GetMediaCenterList() { return m_mediaCenterList; }
	std::vector<RakNet::RakString> & GetLightList() { return m_lightList; }
	std::vector<RakNet::RakString> & GetSpecList() { return m_specList; }
	std::vector<RakNet::RakString> & GetProjectorList() { return m_projectorList; }

	RakNet::RakString GetClientName(const RakNet::SystemAddress &systemAddress);
	RakNet::SystemAddress GetClientAddress(const RakNet::RakString &name);
	bool IsCppClient(const RakNet::RakString &clientName); // 是否是c++客户端

	//接收非RPC传输的数据,模拟RPC接口
	virtual void ReceiveNoRpcData(RakNet::BitStream *bitStream, RakNet::Packet *packet) { ; }

private:
	unsigned char GetPacketIdentifier(RakNet::Packet *p, int index = 0);

	bool Verify(const RakNet::RakString & clientName);
	void UpdateClientInfo(const RakNet::RakString &clientName, const RakNet::SystemAddress &address);
	void ResetClientInfo(const RakNet::SystemAddress &address);


	void SetupClientTypeList(ClientType type, const RakNet::RakString &clientName);
	void AddClientToTypeList(std::vector<RakNet::RakString> &clientList, const RakNet::RakString &clientName);

	void ModifyClientTypeList(const RakNet::RakString &clientName);
	void RemoveClientFromTypeList(std::vector<RakNet::RakString> &clientList, const RakNet::RakString &clientName);

	void SetupCppClientList(LangType type, const RakNet::RakString &clientName);
	void RemoveCppClientFromList(const RakNet::RakString &clientName);


private:
	std::vector<ClientInfo>  m_clientInfoList;

	std::vector<RakNet::RakString>	 m_mediaCenterList; // 同步播放控制客户端列表：一般来说，一个项目中只有一个同步播放控制(MediaCenter)客户端
	std::vector<RakNet::RakString>   m_lightList; // 灯控客户端列表：一般来说，一个项目中只有一个灯控客户端
	std::vector<RakNet::RakString>	 m_specList; // 特技客户端列表：一般来说，一个项目中只有一个特技客户端
	std::vector<RakNet::RakString>	 m_projectorList; // 投影机控制客户端列表：一般来说，一个项目中只有一个投影机控制客户端

	std::vector<RakNet::RakString>   m_cppClientList; // c++客户端列表
};


#endif