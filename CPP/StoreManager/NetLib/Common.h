#pragma once

#ifndef Common_H__
#define Common_H__

const int MAX_CLIENTS = 1000; // 最大支持连接的客户端数量
const int PACKET_SIZE = 1024; //数据包最大字节数
const int RECONNECT_INTERVAL = 2000; // 网络断开之后，重新连接时间间隔
const int UNRELIABLE_TIMEOUT = 2000; // how long to wait before giving up on sending an unreliable message
const int TIMEOUT_TIME = 2000;
const int SERVER_PORT = 60666;
const int SUB_SERVER_PORT = 60667;
const int LOOKUP_INTERVAL = 2000; // 自动广播查找服务器时间间隔，2秒

const int PLC_STATUS_DATA_SIZE = 2048 * 40;

// 网络连接状态
enum NetworkState
{
	// 正在建立连接
	Connecting,
	// 正在验证连接
	Verifying,
	// 已经连接
	Connected,
	// 已经断开连接
	Disconnected
};

// 客户端信息
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

	// Server<->Client:服务端与客户端(主要用于C#客户端)之间发送控制指令
	ID_NET_COMMAND_REQUEST,

	// Client<->Server:客户端(主要用于C#客户端)与服务端之间返回控制指令执行结果
	ID_NET_COMMAND_RESPONSE,

	// Client->Server:客户端上报异常或者错误信息到服务端
	ID_REPORT_CLIENT_ERROR,

	// Client->Server:特技客户端上报PLC状态数据到服务端
	ID_REPORT_PLC_STATUS,

	// Client->Server->Client:特技客户端通过服务端转发PLC状态数据到另一个客户端
	ID_FORWARD_PLC_STATUS,

	// Client->Server->Client: 客户端通过服务端向另一个客户端发送控制指令
	ID_FORWARD_NET_COMMAND_REQUEST,

	// Client->Server->Client: 客户端通过服务端向另一个客户端返回控制指令执行结果
	ID_FORWARD_NET_COMMAND_RESPONSE,

	//多媒体播放客户端（音频和视频）预留ID段200~220，千万不要修改！
	ID_MEDIA_COMMAND_START = 200,
	ID_MEDIA_COMMAND_END = 220,

	//////////////////////////////////// 通用客户端管理指令 end   /////////////////////////////////

	ID_USER_PACKET_ENUM_EX //用户自定义扩展消息
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

// 区分c++语言客户端和c#语言客户端
// c#客户端不支持RPC调用，需要单独处理
enum LangType
{
	Cpp = 1,
	CSharp,
};


#endif // !Common_H__