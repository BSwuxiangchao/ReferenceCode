#pragma once

#ifndef RAKCLIENTPLUGIN_H__
#define RAKCLIENTPLUGIN_H__

#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RPC4Plugin.h"
#include "Common.h"
#include "Export.h"
#include "RPCDefines.h"

class RAK_DLL_EXPORT RakClientPlugin : public RakNet::PluginInterface2
{
public:
	// 派生类重写 OnReceive 函数时，记得先调用父类的函数 RakClientPlugin::OnReceive
	virtual RakNet::PluginReceiveResult OnReceive(RakNet::Packet *packet) override;

	// 当客户端名称和密码匹配，服务端通过验证时，调用此函数
	virtual void OnVerifyConnection(const RakNet::SystemAddress &systemAddress, bool passed) { (void)systemAddress; (void)passed; }

	// 客户端处理服务器转发过来的PLC状态数据
	virtual void OnReceivePLCStatus(RakNet::BitStream *bitStream) { }

	virtual bool OnForwardCommandRequest(const RakNet::RakString &sourceClientName, const RakNet::RakString &request) { return true; }

	virtual void OnForwardCommandResponse(const RakNet::RakString &sourceClientName, const RakNet::RakString &response, bool success) { }
};

#endif