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
	// ��������д OnReceive ����ʱ���ǵ��ȵ��ø���ĺ��� RakClientPlugin::OnReceive
	virtual RakNet::PluginReceiveResult OnReceive(RakNet::Packet *packet) override;

	// ���ͻ������ƺ�����ƥ�䣬�����ͨ����֤ʱ�����ô˺���
	virtual void OnVerifyConnection(const RakNet::SystemAddress &systemAddress, bool passed) { (void)systemAddress; (void)passed; }

	// �ͻ��˴��������ת��������PLC״̬����
	virtual void OnReceivePLCStatus(RakNet::BitStream *bitStream) { }

	virtual bool OnForwardCommandRequest(const RakNet::RakString &sourceClientName, const RakNet::RakString &request) { return true; }

	virtual void OnForwardCommandResponse(const RakNet::RakString &sourceClientName, const RakNet::RakString &response, bool success) { }
};

#endif