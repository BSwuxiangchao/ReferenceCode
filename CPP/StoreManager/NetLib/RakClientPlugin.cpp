#include "RPCDefines.h"
#include "RakClientPlugin.h"

RakNet::PluginReceiveResult RakClientPlugin::OnReceive(RakNet::Packet * packet)
{
	switch (packet->data[0])
	{
	case ID_LOGIN_RESPONSE:
	{
		// 解析服务器返回的身份验证结果
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(1);
		BYTE result = 0;
		bsIn.Read(result);
		if (result == 1)
		{
			OnVerifyConnection(packet->systemAddress, true);
		}
		else
		{
			OnVerifyConnection(packet->systemAddress, false);
		}
		break;
	}
	/* 
	 * 已经直接通过RPC调用处理 ID_FORWARD_PLC_STATUS 消息
	case ID_FORWARD_PLC_STATUS:
	{
		BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(1);

		OnReceivePLCStatus(&bsIn);

		break;
	}
	*/
	case ID_FORWARD_NET_COMMAND_REQUEST:
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(1);

		// 采用 ReadAlignedBytes 方法, 同时兼容C#、C++客户端中文字符问题
		RakNet::RakString sourceClientName;
		BYTE sourceBytes[64] = { 0 };
		int length = 0;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(sourceBytes, length);
		sourceClientName = sourceBytes;

		BYTE requestBytes[128] = { 0 };
		RakNet::RakString request;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(requestBytes, length);
		request = requestBytes;

		// 处理通过服务端转发过来的另一客户端的控制指令
		bool success = OnForwardCommandRequest(sourceClientName, request);
		
		// 返回控制指令执行结果到源客户端
		RakNet::BitStream bsOut;
		bsOut.Write((BYTE)ID_FORWARD_NET_COMMAND_RESPONSE);

		RakNet::RakString targetClientName = sourceClientName;
		length = targetClientName.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)targetClientName.C_String(), length);

		RakNet::RakString response = request;
		length = response.GetLength();
		bsOut.Write(length);
		bsOut.WriteAlignedBytes((const unsigned char *)response.C_String(), length);

		bsOut.Write(success);
		
		GetRakPeerInterface()->Send(&bsOut, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, packet->systemAddress, false);

		break;
	}
	case ID_FORWARD_NET_COMMAND_RESPONSE:
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(1);

		// 采用 ReadAlignedBytes 方法, 兼容C#客户端中文名称问题
		RakNet::RakString sourceClientName;
		BYTE sourceBytes[64] = { 0 };
		int length = 0;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(sourceBytes, length);
		sourceClientName = sourceBytes;

		BYTE responseBytes[128] = { 0 };
		RakNet::RakString response;
		bsIn.Read(length);
		bsIn.ReadAlignedBytes(responseBytes, length);
		response = responseBytes;

		bool success = false;
		bsIn.Read(success);
		OnForwardCommandResponse(sourceClientName, response, success);

		break;
	}

	default:
		break;
	}
	return RakNet::RR_CONTINUE_PROCESSING;
}

