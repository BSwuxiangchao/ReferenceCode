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

	/// ����ͻ���(��Ҫ��C#�ͻ���, C++�ͻ��˲���RPC����)���͹����Ŀ���ָ��
	virtual void OnNetCommandRequest(RakNet::BitStream *bitStream) { }

	/// ����ͻ���(��Ҫ��C#�ͻ���, C++�ͻ��˲���RPC����)���ص�ָ��ִ�н��
	virtual void OnNetCommandResponse(RakNet::BitStream *bitStream) { }

	/// ����ͻ���(��Ҫ��C#�ͻ���, C++�ͻ��˲���RPC����)���ص��쳣���ߴ�����Ϣ
	virtual void OnReceiveClientError(RakNet::BitStream *bitStream) { }

	virtual void OnReceivePLCStatus(RakNet::BitStream *bitStream) { }

	/// ����ͻ���ת������ָ��
	/// sourceClientName: ����ָ�����Դ�ͻ���
	/// targetClientName: ����ָ��Ľ��տͻ���
	/// request: ����ָ��
	/// success: �Ƿ�ת���ɹ�
	virtual void OnForwardCommandRequest(const RakNet::RakString &sourceClientName, const RakNet::RakString &targetClientName, const RakNet::RakString &request, bool success) { }

	/// ����ͻ���ת���Ŀ���ָ��ִ�н��
	/// sourceClientName: ִ�н������Դ�ͻ���
	/// targetClientName: ִ�н���Ľ��տͻ���
	/// response: ִ��ָ��/���
	/// success: �Ƿ�ִ�гɹ�
	virtual void OnForwardCommandResponse(const RakNet::RakString &sourceClientName, const RakNet::RakString &targetClientName, const RakNet::RakString &response, bool success) { }

	void SetClientInfoList(const std::vector<ClientInfo> &clientInfoList) { m_clientInfoList = clientInfoList; }
	std::vector<ClientInfo> & GetClientInfoList() { return m_clientInfoList; }
	std::vector<RakNet::RakString> & GetMediaCenterList() { return m_mediaCenterList; }
	std::vector<RakNet::RakString> & GetLightList() { return m_lightList; }
	std::vector<RakNet::RakString> & GetSpecList() { return m_specList; }
	std::vector<RakNet::RakString> & GetProjectorList() { return m_projectorList; }

	RakNet::RakString GetClientName(const RakNet::SystemAddress &systemAddress);
	RakNet::SystemAddress GetClientAddress(const RakNet::RakString &name);
	bool IsCppClient(const RakNet::RakString &clientName); // �Ƿ���c++�ͻ���

	//���շ�RPC���������,ģ��RPC�ӿ�
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

	std::vector<RakNet::RakString>	 m_mediaCenterList; // ͬ�����ſ��ƿͻ����б�һ����˵��һ����Ŀ��ֻ��һ��ͬ�����ſ���(MediaCenter)�ͻ���
	std::vector<RakNet::RakString>   m_lightList; // �ƿؿͻ����б�һ����˵��һ����Ŀ��ֻ��һ���ƿؿͻ���
	std::vector<RakNet::RakString>	 m_specList; // �ؼ��ͻ����б�һ����˵��һ����Ŀ��ֻ��һ���ؼ��ͻ���
	std::vector<RakNet::RakString>	 m_projectorList; // ͶӰ�����ƿͻ����б�һ����˵��һ����Ŀ��ֻ��һ��ͶӰ�����ƿͻ���

	std::vector<RakNet::RakString>   m_cppClientList; // c++�ͻ����б�
};


#endif