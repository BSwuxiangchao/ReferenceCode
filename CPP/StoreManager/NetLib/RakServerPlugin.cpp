#include <vector>
#include "RakServerPlugin.h"

RakNet::PluginReceiveResult RakServerPlugin::OnReceive(RakNet::Packet * p)
{
	BYTE msgID = GetPacketIdentifier(p, 0);
	switch (msgID)
	{
	case ID_LOGIN_REQUEST:
	{
		// 客户端身份验证
		RakNet::BitStream bsIn(p->data, p->length, false);
		bsIn.IgnoreBytes(1);

		int clientType = -1;
		int langType = 1;

		//bsIn.ReadCompressed(clientName);
		// 采用 ReadAlignedBytes 方法, 兼容C#客户端中文名称问题
		RakNet::RakString clientName;
		BYTE bytes[64] = { 0 };
		int length = 0;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(bytes, length);
		clientName = bytes;

		bsIn.Read(clientType);
		bsIn.Read(langType);

		RakNet::BitStream bsOut;
		bsOut.Write((BYTE)ID_LOGIN_RESPONSE);
		// 客户端名字匹配，验证通过
		if (Verify(clientName))
		{
			UpdateClientInfo(clientName.C_String(), p->systemAddress);

			// 向客户端发送验证结果：1 验证通过；0 未通过
			bsOut.Write((BYTE)1);

			GetRakPeerInterface()->Send(&bsOut, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, p->systemAddress, false);
			SetupClientTypeList((ClientType)clientType, clientName);
			SetupCppClientList((LangType)langType, clientName);

			OnVerifyConnection(clientName, true);
		}
		else
		{
			// 向客户端发送验证结果：1 验证通过；0 未通过
			bsOut.Write((BYTE)0);
			GetRakPeerInterface()->Send(&bsOut, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, p->systemAddress, false);

			GetRakPeerInterface()->CloseConnection(p->systemAddress, true);

			OnVerifyConnection(clientName, false);
		}

		break;
	}
	case ID_NET_COMMAND_REQUEST:
	{
		RakNet::BitStream bs(p->data, p->length, true);

		OnNetCommandRequest(&bs);
		break;
	}
	case ID_NET_COMMAND_RESPONSE:
	{
		RakNet::BitStream bs(p->data, p->length, true);

		OnNetCommandResponse(&bs);

		break;
	}
	case ID_FORWARD_NET_COMMAND_REQUEST:
	{
		RakNet::BitStream bsIn(p->data, p->length, false);
		bsIn.IgnoreBytes(1);

		// 采用 ReadAlignedBytes 方法, 同时兼容C#、C++客户端中文字符问题
		RakNet::RakString targetClientName;
		BYTE targetBytes[64] = { 0 };
		int length = 0;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(targetBytes, length);
		targetClientName = targetBytes;

		BYTE requestBytes[128] = { 0 };
		RakNet::RakString request;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(requestBytes, length);
		request = requestBytes;

		RakNet::RakString sourceClientName = GetClientName(p->systemAddress);

		RakNet::BitStream bsOut;
		bsOut.Write((BYTE)ID_FORWARD_NET_COMMAND_REQUEST);

		length = sourceClientName.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)sourceClientName.C_String(), length);

		length = request.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)request.C_String(), length);

		RakNet::SystemAddress targetAddress = GetClientAddress(targetClientName.C_String());
		uint32_t result = GetRakPeerInterface()->Send(&bsOut, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, targetAddress, false);
		if (result == 0)
		{
			// 提取指令信息后，在总控进行日志输出等处理
			OnForwardCommandRequest(sourceClientName, targetClientName, request, false);
		}
		else
		{
			OnForwardCommandRequest(sourceClientName, targetClientName, request, true);
		}

		break;
	}
	case ID_FORWARD_NET_COMMAND_RESPONSE:
	{
		RakNet::BitStream bsIn(p->data, p->length, false);
		bsIn.IgnoreBytes(1);

		// 采用 ReadAlignedBytes 方法, 兼容C#客户端中文名称问题
		RakNet::RakString targetClientName;
		BYTE targetBytes[64] = { 0 };
		int length = 0;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(targetBytes, length);
		targetClientName = targetBytes;

		BYTE responseBytes[128] = { 0 };
		RakNet::RakString response;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(responseBytes, length);
		response = responseBytes;

		bool success = false;
		bsIn.Read(success);

		RakNet::RakString sourceClientName = GetClientName(p->systemAddress);

		OnForwardCommandResponse(sourceClientName, targetClientName, response, success);

		RakNet::BitStream bsOut;
		bsOut.Write((BYTE)ID_FORWARD_NET_COMMAND_RESPONSE);

		length = sourceClientName.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)sourceClientName.C_String(), length);

		length = response.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)response.C_String(), length);

		bsOut.Write(success);

		RakNet::SystemAddress targetAddress = GetClientAddress(targetClientName.C_String());
		GetRakPeerInterface()->Send(&bsOut, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, targetAddress, false);

		break;
	}
	case ID_REPORT_CLIENT_ERROR:
	{
		RakNet::BitStream bs(p->data, p->length, false);
		bs.IgnoreBytes(1);

		OnReceiveClientError(&bs);

		break;
	}
	case ID_REPORT_PLC_STATUS:
	{
		RakNet::BitStream bs(p->data, p->length, false);

		OnReceivePLCStatus(&bs);

		break;
	}
	case 168:	//非RPC的数据，ID固定为168，外部再根据command跳转到RPC对应的接口
	{
		//接收非RPC数据

		RakNet::BitStream bsIn(p->data, p->length, false);
		bsIn.IgnoreBytes(1);

		ReceiveNoRpcData(&bsIn,p);
		break;
	}
	default:
		break;
	}
	return RakNet::RR_CONTINUE_PROCESSING;
}

RakNet::RakString RakServerPlugin::GetClientName(const RakNet::SystemAddress & systemAddress)
{
	std::vector<ClientInfo> clientInfoList = GetClientInfoList();
	for (auto item : clientInfoList)
	{
		if (item.address == systemAddress)
		{
			return item.name;
		}
	}

	return "";
}

RakNet::SystemAddress RakServerPlugin::GetClientAddress(const RakNet::RakString & name)
{
	std::vector<ClientInfo> clientInfoList = GetClientInfoList();
	for (auto& item : clientInfoList)
	{
		if (item.name == name)
			return item.address;
	}

	return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}

bool RakServerPlugin::IsCppClient(const RakNet::RakString &clientName)
{
	for (auto item : m_cppClientList)
	{
		if (item.StrICmp(clientName) == 0)
		{
			return true;
		}
	}

	return false;
}

void RakServerPlugin::OnClosedConnection(const RakNet::SystemAddress & systemAddress, RakNet::RakNetGUID rakNetGUID, RakNet::PI2_LostConnectionReason lostConnectionReason)
{
	(void)rakNetGUID;
	(void)lostConnectionReason;

	RakNet::RakString clientName = GetClientName(systemAddress);
	ModifyClientTypeList(clientName);
	RemoveCppClientFromList(clientName);

	ResetClientInfo(systemAddress);
}


unsigned char RakServerPlugin::GetPacketIdentifier(RakNet::Packet *p, int index /*= 0*/)
{
	if (p == nullptr) return 255;

	if ((unsigned char)p->data[index] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[index];
}

bool RakServerPlugin::Verify(const RakNet::RakString & clientName)
{
	for (auto item : m_clientInfoList)
	{
		if (clientName.StrICmp(item.name) == 0)
		{
			return true;
		}
	}

	return false;
}

void RakServerPlugin::UpdateClientInfo(const RakNet::RakString &clientName, const RakNet::SystemAddress &address)
{
	for (auto& item : m_clientInfoList)
	{
		if (item.name.StrICmp(clientName) == 0)
		{
			item.isConnected = 1;
			item.address = address;
		}
	}
}

void RakServerPlugin::ResetClientInfo(const RakNet::SystemAddress & address)
{
	for (auto& item : m_clientInfoList)
	{
		if (item.address == address)
		{
			item.isConnected = 0;
			item.address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
	}
}

void RakServerPlugin::SetupClientTypeList(ClientType type, const RakNet::RakString &clientName)
{
	switch (type)
	{
	case MediaCenter:
	{
		AddClientToTypeList(m_mediaCenterList, clientName);
		break;
	}
	case Video:
		break;
	case Audio:
		break;
	case Photo:
		break;
	case Light:
	{
		AddClientToTypeList(m_lightList, clientName);
		break;
	}
	case Spec:
	{
		AddClientToTypeList(m_specList, clientName);
		break;
	}
	case Monitor:
		break;
	case Projector:
	{
		AddClientToTypeList(m_projectorList, clientName);
		break;
	}
	default:
		break;
	}
}

void RakServerPlugin::AddClientToTypeList(std::vector<RakNet::RakString> &clientList, const RakNet::RakString &clientName)
{
	bool existed = false;
	for (auto item : clientList)
	{
		if (clientName.StrICmp(item) == 0)
		{
			existed = true;
			break;
		}
	}

	if (!existed)
		clientList.push_back(clientName);
}

void RakServerPlugin::ModifyClientTypeList(const RakNet::RakString &clientName)
{
	RemoveClientFromTypeList(m_mediaCenterList, clientName);
	RemoveClientFromTypeList(m_lightList, clientName);
	RemoveClientFromTypeList(m_specList, clientName);
	RemoveClientFromTypeList(m_projectorList, clientName);
}

void RakServerPlugin::RemoveClientFromTypeList(std::vector<RakNet::RakString> &clientList, const RakNet::RakString &clientName)
{
	for (auto iter = clientList.begin(); iter != clientList.end(); )
	{
		if ((*iter).StrICmp(clientName) == 0)
		{
			iter = clientList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void RakServerPlugin::SetupCppClientList(LangType type, const RakNet::RakString &clientName)
{
	if (type == Cpp)
	{
		bool existed = false;
		for (auto item : m_cppClientList)
		{
			if (clientName.StrICmp(item) == 0)
			{
				existed = true;
				break;
			}
		}

		if (!existed)
			m_cppClientList.push_back(clientName);
	}
}

void RakServerPlugin::RemoveCppClientFromList(const RakNet::RakString &clientName)
{
	for (auto iter = m_cppClientList.begin(); iter != m_cppClientList.end(); )
	{
		if ((*iter).StrICmp(clientName) == 0)
		{
			iter = m_cppClientList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

