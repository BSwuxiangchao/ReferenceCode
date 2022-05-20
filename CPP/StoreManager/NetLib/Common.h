#pragma once

#ifndef Common_H__
#define Common_H__

const int MAX_CLIENTS = 1000; // ���֧�����ӵĿͻ�������
const int PACKET_SIZE = 1024; //���ݰ�����ֽ���
const int RECONNECT_INTERVAL = 2000; // ����Ͽ�֮����������ʱ����
const int UNRELIABLE_TIMEOUT = 2000; // how long to wait before giving up on sending an unreliable message
const int TIMEOUT_TIME = 2000;
const int SERVER_PORT = 60666;
const int SUB_SERVER_PORT = 60667;
const int LOOKUP_INTERVAL = 2000; // �Զ��㲥���ҷ�����ʱ������2��

const int PLC_STATUS_DATA_SIZE = 2048 * 40;

// ��������״̬
enum NetworkState
{
	// ���ڽ�������
	Connecting,
	// ������֤����
	Verifying,
	// �Ѿ�����
	Connected,
	// �Ѿ��Ͽ�����
	Disconnected
};

// �ͻ�����Ϣ
struct ClientInfo
{
	class RakNet::RakString name;
	int isConnected;
	DWORD ping;
	RakNet::SystemAddress address;

	ClientInfo()
	{
		name = "";
		isConnected = 0;
		ping = 0;
		address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	}
};

enum NetworkMessageTypes
{
	ID_LOGIN_REQUEST = 135, // 135 = ID_USER_PACKET_ENUM + 1,
	ID_LOGIN_RESPONSE,
	ID_LOGOUT_REQUEST,
	ID_LOGOUT_RESPONSE,

	// Server<->Client:�������ͻ���(��Ҫ����C#�ͻ���)֮�䷢�Ϳ���ָ��
	ID_NET_COMMAND_REQUEST,

	// Client<->Server:�ͻ���(��Ҫ����C#�ͻ���)������֮�䷵�ؿ���ָ��ִ�н��
	ID_NET_COMMAND_RESPONSE,

	// Client->Server:�ͻ����ϱ��쳣���ߴ�����Ϣ�������
	ID_REPORT_CLIENT_ERROR,

	// Client->Server:�ؼ��ͻ����ϱ�PLC״̬���ݵ������
	ID_REPORT_PLC_STATUS,

	// Client->Server->Client:�ؼ��ͻ���ͨ�������ת��PLC״̬���ݵ���һ���ͻ���
	ID_FORWARD_PLC_STATUS,

	// Client->Server->Client: �ͻ���ͨ�����������һ���ͻ��˷��Ϳ���ָ��
	ID_FORWARD_NET_COMMAND_REQUEST,

	// Client->Server->Client: �ͻ���ͨ�����������һ���ͻ��˷��ؿ���ָ��ִ�н��
	ID_FORWARD_NET_COMMAND_RESPONSE,

	//��ý�岥�ſͻ��ˣ���Ƶ����Ƶ��Ԥ��ID��200~220��ǧ��Ҫ�޸ģ�
	ID_MEDIA_COMMAND_START = 200,
	ID_MEDIA_COMMAND_END = 220,

	//////////////////////////////////// ͨ�ÿͻ��˹���ָ�� end   /////////////////////////////////

	ID_USER_PACKET_ENUM_EX //�û��Զ�����չ��Ϣ
};

enum ClientType
{
	Video = 1,
	Audio,
	Photo,
	Light,
	Spec,
	Projector,
	MediaCenter,
	Monitor,
	Other,
};

// ����c++���Կͻ��˺�c#���Կͻ���
// c#�ͻ��˲�֧��RPC���ã���Ҫ��������
enum LangType
{
	Cpp = 1,
	CSharp,
};


#endif // !Common_H__