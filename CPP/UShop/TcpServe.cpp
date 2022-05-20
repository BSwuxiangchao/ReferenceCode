#pragma once
#include"stdafx.h"
#include"TcpServer.hpp"
#include"ToolHelp.h"
#include"WarningDlg.h"
#include "PlcFace.h"
#include"PlcFaceDlg.h"
#include"ShowAddGoodsInfo.h"

ClientSocket::ClientSocket(SOCKET sockfd /*= INVALID_SOCKET*/)
{
	_sockfd = sockfd;
	memset(_szMsgBuff, 0, sizeof(_szMsgBuff));
	_lastPos = 0;
	_PreHeartTime = time(NULL);
}

TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_sock2= INVALID_SOCKET;
	_clients.clear();
	_serverSendCmd.clear();
	m_th = CreateThreadS();
	m_pickUpGoodsService.registerPickResultSlot(BOOL_Slot(&TcpServer::RobotArmPutGoodsEnd,this,_1));

}
TcpServer::~TcpServer()
{
	m_isStopRouine = true;
	m_pickUpGoodsService.cancelAllSlot();
	//m_pickUpGoodsService.~UshopService();
	Close();
	for (auto &it : _serverSendCmd)
	{
		if (it.second.first)
		{
			delete it.second.first;
			it.second.first = nullptr;
		}
	}
	_serverSendCmd.clear();
	if (m_th)
	{
		Join(m_th);
	}
}
void TcpServer::Close()
{
	//关闭套接字
	if (_sock != INVALID_SOCKET|| _sock2 != INVALID_SOCKET)
	{
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			closesocket(_clients[n]->sockfd());
			delete _clients[n];
			_clients[n] = nullptr;
		}
		if(_sock != INVALID_SOCKET)
			closesocket(_sock);
		if (_sock2 != INVALID_SOCKET)
			closesocket(_sock2);
	}

	//关闭windows socket环境
	WSACleanup();
	_clients.clear();
}
//初始化socket
SOCKET TcpServer::IntiSocket()
{
	//启动win Sock 2.x环境
	WSADATA wsadata;
	memset(&wsadata, 0x00, sizeof(wsadata));
	auto retStartup = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (retStartup != 0)
	{
		SZL_ERROR("ws2_32.lib load error.");
		exit(-1);
	}
	if (INVALID_SOCKET != _sock|| INVALID_SOCKET != _sock2)
	{
		SZL_INFO("<socket={0}>,<socket2={1}>关闭旧连接", (int)_sock, (int)_sock2);
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		SZL_ERROR("建立socket失败");
	}
	else
	{
		SZL_INFO("建立<socket={0}成功.", (int)_sock);
	}
	_sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock2)
	{
		SZL_ERROR("建立socket失败");
	}
	else
	{
		SZL_INFO("建立<socket={0}成功.", (int)_sock2);
	}
	return _sock;
}
//绑定端口号
int TcpServer::Bind()
{
	//bind 绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);  //
	if (_ip!="0")
	{
		_sin.sin_addr.S_un.S_addr = inet_addr(_ip.c_str());
	}
	else
	{
		_sin.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1")
	}
	int ret = ::bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("绑定网络端口<{0}>失败,错误码[{1}]", (int)_port, GetLastError());
	}
	else
	{
		SZL_INFO("绑定网络端口<{0}>成功", (int)_port);
	}
	_sin.sin_port = htons(_port2);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("绑定网络端口<{0}>失败,错误码[{1}]", (int)_port2, GetLastError());
	}
	else
	{
		SZL_INFO("绑定网络端口<{0}>成功", (int)_port2);
	}
	ret= ::bind(_sock2, (sockaddr*)&_sin, sizeof(sockaddr_in));
	return ret;
}
//监听网络端口
int TcpServer::Listen(int n)
{
	int ret = listen(_sock, n);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("<socket={0}>错误，监听网络端口失败", (int)_sock);
	}
	else
	{
		SZL_INFO("<socket={0}>成功，监听网络端口成功", (int)_sock);
	}
	ret = listen(_sock2, n);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("<socket={0}>错误，监听网络端口失败", (int)_sock2);
	}
	else
	{
		SZL_INFO("<socket={0}>成功，监听网络端口成功", (int)_sock2);
	}
	return ret;
}
SOCKET TcpServer::Accept()
{
	//accept等待接收客户端连接
	sockaddr_in ClientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(_sock, (sockaddr*)&ClientAddr, &nAddrLen);
	if (cSock == INVALID_SOCKET)
	{
		SZL_ERROR("<socket={0}>错误，接收无效客户端SOCKET", (int)_sock);
	}
	else
	{
		ClientSocket* cSoc = new ClientSocket(cSock);
		cSoc->SetSockId(SockIdType::PLC_CLIENT);
		_clients.push_back(cSoc);
		SZL_INFO("<socket={0}>新客户端加入：socket={1},IP={2}", (int)_sock, (int)cSock, inet_ntoa(ClientAddr.sin_addr));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		p->SetPlcConnectState(true);
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "plc客户端连接成功，IP:%s,端口:%d", inet_ntoa(ClientAddr.sin_addr), ClientAddr.sin_port);
		p->NotifyMsg(tBuf);
		if (m_IsStartUp)
		{
			resetPos();
			m_IsStartUp = false;
		}
	}
	return cSock;
}
SOCKET TcpServer::Accept2()
{
	//accept等待接收客户端连接
	sockaddr_in ClientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(_sock2, (sockaddr*)&ClientAddr, &nAddrLen);
	if (cSock == INVALID_SOCKET)
	{
		SZL_ERROR("<socket={0}>错误，接收无效客户端SOCKET", (int)_sock2);
	}
	else
	{
		ClientSocket* cSoc = new ClientSocket(cSock);
		cSoc->SetSockId(SockIdType::POS_CLIENT);
		_clients.push_back(cSoc);
		SZL_INFO("<socket={0}>新客户端连接：socket={1},IP={2}", (int)_sock2, (int)cSock, inet_ntoa(ClientAddr.sin_addr));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		//p->SetPlcConnectState(true);
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "plc位置上报专用端口连接成功，IP:%s,端口:%d", inet_ntoa(ClientAddr.sin_addr), ClientAddr.sin_port);
		p->NotifyMsg(tBuf);
	}
	return cSock;
}
bool TcpServer::OnRun()
{
	if (isRun())
	{
		//伯克利套接字 BSD socket
		//描述符集合
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//清理集合
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//加入集合
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		FD_SET(_sock2, &fdRead);
		FD_SET(_sock2, &fdWrite);
		FD_SET(_sock2, &fdExp);
		SOCKET maxSock = _sock;

		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(_clients[n]->sockfd(), &fdRead);
			if (maxSock < _clients[n]->sockfd())
			{
				maxSock = _clients[n]->sockfd();
			}
		}
		if (maxSock < _sock2)
			maxSock = _sock2;
		//nfds是一个整数值，是指fd_set集合的所有描述符的范围，而不是数量
		//既是所有文件描述符最大值+1，在windows参数可以0
		timeval t = { 1,0 }; //设置超时时间1秒
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			SZL_INFO("select任务结束");
			Close();
			return false;
		}
		//判断描述符是否在集合中
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
		//判断描述符是否在集合中
		if (FD_ISSET(_sock2, &fdRead))
		{
			FD_CLR(_sock2, &fdRead);
			Accept2();
		}
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			//心跳超过30秒，踢出客户端
			if (_clients[n]->GetSockId() == SockIdType::PLC_CLIENT)
			{
				if (disConnectClientTimeOut(_clients[n]))
				{
					continue;
				}
			}
			if (FD_ISSET(_clients[n]->sockfd(), &fdRead))
			{
				if (-1 == RecvData(_clients[n]))
				{
					auto itCl = _clients.begin() + n;
					if (itCl != _clients.end())
					{
						delete _clients[n];
						_clients[n] = nullptr;
						_clients.erase(itCl);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool TcpServer::isRun()
{
	return _sock != INVALID_SOCKET;
}
int TcpServer::RecvData(ClientSocket* pClient)
{
	//收到数据重新设置心跳时间
	time_t ttm = time(NULL);
	pClient->setHeartTime(ttm);
	//接收客户端数据
	memset(_szRecv, 0x00, sizeof(_szRecv));
	int nLen = (int)recv(pClient->sockfd(), _szRecv, RECV_BUFF_SIZE, 0);
	//将接收到的数据拷贝到消息缓冲区
	if (nLen <= 0)
	{
		SZL_INFO("客户端<socket={0}>已退出，任务结束", (int)pClient->sockfd());
		return -1;
	}
	std::string sStr = ToolHelp::GetHexString(_szRecv, nLen);
	SZL_DEBUG("客户端<socket={0}>发送的消息:{1}", (int)pClient->sockfd(), sStr.c_str());
	memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);
	//消息缓冲区的尾部数据位置后移
	pClient->setLastPos(pClient->getLastPos() + nLen);
	//判断消息缓冲区的数据长度大于DataHeader消息头长度
	while (pClient->getLastPos() >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)pClient->msgBuf();
		//判断消息缓冲区的数据长度大于消息长度
		if (pClient->getLastPos() >= header->_len)
		{
			//剩余未处理消息缓冲区数据的长度
			int nSize = pClient->getLastPos() - header->_len;
			//处理网络信息
			OnNetMsg(pClient->sockfd(), pClient->msgBuf());
			//将消息缓冲区剩余数据前移
			memcpy(pClient->msgBuf(), pClient->msgBuf() + header->_len, nSize);
			//消息缓冲区尾部数据位置前移
			pClient->setLastPos(nSize);
		}
		else
		{
			//消息缓冲区属于数据不够一条完整消息
			break;
		}
	}
	return 0;
}
void TcpServer::OnNetMsg(SOCKET cSock, char* str)
{
	if ((std::string)str == "")
	{
		SZL_WARN("接收到的字符串为空");
		return;
	}
	DataHeader* header = (DataHeader*)str;
	SZL_DEBUG("客户端发送的消息类型 _msgType={0}", (int)header->_msgType);
	switch (header->_msgType)
	{
	case CLIENT_MSG_TYPE_CMD::HEART_BEAT:
	{
		heartBeat(cSock, str);
	}
	break;
	case CLIENT_MSG_TYPE_CMD::LOGIN:
	{
		Login(cSock, str);
	}
	break;
	case CLIENT_MSG_TYPE_CMD::REPORT:
	{
		Report(cSock, str);
	}
	break;
	case CLIENT_MSG_TYPE_CMD::CLIENT_REPLY:
	{
		ClientReponse(cSock, str);
	}
	break;
	case CLIENT_MSG_TYPE_CMD::JXB_REPLY:
	{
		//JxbReponse(cSock, str);
	}
	break;
	default:
	{
		SZL_INFO("<socket={0}>收到未定义消息，数据长度：{1}", (int)cSock, (int)header->_len);
	}
	break;
	}
}
int TcpServer::SendData(SOCKET cSock, DataHeader* header)
{
	if (isRun() && header)
	{
		//向plc发送数据长度固定为12
		std::string sStr = ToolHelp::GetHexString((char*)header, 12);
		SZL_INFO("服务端发送的数据，{0}", sStr.c_str());
		return send(cSock, (const char*)header, 12, 0);
	}
	return 0;
}
void TcpServer::SendDataToAll(DataHeader* header)
{
	if (isRun() && header)
	{
		
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			SendData(_clients[n]->sockfd(), header);
		}
	}
}
void TcpServer::Report(SOCKET cSock, char* str)
{
	ClientReport* report = (ClientReport*)str;
	SZL_DEBUG("客户端上报类型：_reportType={0}", (int)report->_reportType);
	switch (report->_reportType)
	{
	case CLIENT_MSG_REPORT_TYPE_CMD::STATE_REPORT:
	{
		stateReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::BOX_IN_REPORT:
	{
		boxInReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::BOX_OUT_REPORT:
	{
		boxOutReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::BOX_TRANSFOR_IN_REPORT:
	{
		boxTransforInReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::BOX_TRANSFOR_OUT_REPORT:
	{
		boxTransforOutReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::CUSTOMER_TAKE_END_REPORT:
	{
		customerTakeEndReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::ROBARM_PUT_END_REPORT:
	{
		robarmPutEndReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::ROBARM_TAKE_END_REPORT:
	{
		robarmTakeEndReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::ROBARM_TAKE_REPORT:
	{
		robarmTakeReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::ADD_GOODS_OUT_REPORT:
	{
		AddGoodsBoxOutReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::ADD_GOODS_IN_REPORT:
	{
		AddGoodsBoxInReport(cSock, str);
	}
	break;
	case CLIENT_MSG_REPORT_TYPE_CMD::XYZ_POS_REPORT:
	{
		XYZPosReport(cSock, str);
	}
	break;
	default:
	{
		SZL_WARN("客户端上报类型:{0} 未定义！！！！", (int)report->_reportType);
	}
	break;
	}
}
void TcpServer::heartBeat(SOCKET cSock, char* str)
{
	SZL_INFO("收到plc发送的心跳消息");

	ClientHeartBeat* beat = (ClientHeartBeat*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::HEART_BEAT_REP;
	memcpy(rep->_key, beat->_key, sizeof(beat->_key));
	rep->_devType = beat->_devType;
	rep->_devId = beat->_devId;

	if (SOCKET_ERROR == SendData(cSock, rep))
	{
		SZL_ERROR("回复心跳失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::Login(SOCKET cSock, char* str)
{
	SZL_INFO("收到plc发送的登录消息");

	ClientLogin* login = (ClientLogin*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::LOGIN_REP;
	memcpy(rep->_key, login->_key, sizeof(login->_key));
	rep->_devType = login->_devType;
	rep->_devId = login->_devId;
	_devInfo._devId = login->_devId;
	_devInfo._devType = login->_devType;
	//设置身份标识
	for (auto &it : _clients)
	{
		if (cSock == it->sockfd())
		{
			it->SetSockId(SockIdType::PLC_CLIENT);
		}
	}
	if (SOCKET_ERROR == SendData(cSock, rep))
	{
		SZL_ERROR("回复登录失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::stateReport(SOCKET sock, char* str)
{
	SZL_INFO("收到plc发送的设备状态上报消息");

	ClientStateReport* pReport = (ClientStateReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	for (int n = (int)_clients.size() - 1; n >= 0; n--)
	{
		if (_clients[n]->sockfd() == sock)
		{
			_clients[n]->_devState._abnormalState = pReport->_abnormalState;
			_clients[n]->_devState._controlMode = pReport->_controlMode;
			_clients[n]->_devState._devState = pReport->_devState;
			/*
			char _controlMode;				 //控制模式,1：自动模式，2：手动模式
			char _devState;					 //设备状态,1：空闲，2：忙碌
			char _abnormalState;			 //异常状态 1：正常 2：异常
			*/
			CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
			p->SetPlcStatePlcShow(pReport);
			char tBuf[1024] = { 0 };
			sprintf_s(tBuf, "收到plc设备状态上报,控制模式:[%d],设备状态:[%d],异常状态:[%d]", (int)pReport->_controlMode, (int)pReport->_devState, (int)pReport->_abnormalState);
			p->NotifyMsg(tBuf);
			std::string str= pReport->_warningMsg;
			WarningHandle(pReport->_warningMsg);
			break;
		}
	}
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复设备状态上报失败，错误码{0}", WSAGetLastError());
	}
	if (m_IsBoxIn)
	{
		if (!m_isBtnBox)
		{
			bool ret = ContOutBox();
			if (!ret)
			{
				m_IsOutGoodsEnd = true;
				SZL_INFO("所有出货完成，更新数据库******************！");
				CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
				//更新数据库
				p->UpdateOutGoodsInfo();
				p->NotifyMsg("所有出货完成，更新数据库******************！");
				//发送各商品库存信息到总控
				p->UpdateStockToServer(2);
			}
		}
		m_isBtnBox = false;
		m_IsBoxIn = false;
	}
	if (m_AddBoxIn)
	{
		//继续推出箱子补货
		if (/*!m_IsAIstoreMode*/1)
		{
			if (!m_isBtnBox)
			{
				bool ret = ContinueOutBox();
				if (!ret && !m_isBtnBox)
				{
					CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
					p->NotifyMsg("所有补货完成！！");
					SZL_INFO("所有补货完成！！");
					SZL_INFO("更新库存到后台！！");
					p->UpdateInGoodsInfoToBg(0);
					//发送各商品库存信息到总控
					p->UpdateStockToServer(1);
					((CPlcFaceApp*)p->GetParentMain())->OnExitStorageMode();
					m_IsLoadGoodsEnd = true;
				}
			}
		}
		m_AddBoxIn = false;
		m_isBtnBox = false;
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::boxOutReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱出库完成上报消息");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的货箱出库完成上报消息");
	ClientBoxOutReport* pReport = (ClientBoxOutReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	int len = SendData(sock, rep);
	if (SOCKET_ERROR == len)
	{
		SZL_ERROR("回复货箱出库完成上报失败，错误码{0}", WSAGetLastError());
	}
	m_IsBoxEx = true;
	auto itB = m_positionInfoList.begin();
	if (itB != m_positionInfoList.end())
	{
		auto itBcell = itB->vecCellInfo.begin();
		if (itBcell != itB->vecCellInfo.end())
		{
			//下发格子号，位置等信息给机械臂，取货
			SendCellInfoToRobot(itB, itBcell);
		}
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::boxInReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱入库完成上报消息");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的货箱入库完成上报消息");
	ClientBoxInReport* pReport = (ClientBoxInReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复货箱入库完成上报失败，错误码{0}", WSAGetLastError());
	}
	m_IsBoxIn = true;
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmTakeReport(SOCKET sock, char* str)
{
	SZL_INFO("收到plc发送的机械臂请求取货上报消息");
	RobarmTakeReport* pReport = (RobarmTakeReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复机械臂请求取货上报失败，错误码{0}", WSAGetLastError());
	}
	if (m_positionInfoList.size() == 0)
	{
		return;
	}
	auto itB = m_positionInfoList.begin();
	auto itBcell = itB->vecCellInfo.begin();

	//if (itBcell->outputGoodsNum <= 0)
	//{
	//	itB->vecCellInfo.erase(itBcell);
	//}
	//if (itB->vecCellInfo.size() == 0)
	//{
	//	//箱子推进去
	//	goodsBoxIn(m_BInNum);
	//	m_positionInfoList.erase(itB);
	//}
	//if (m_positionInfoList.size() == 0)
	//{
	//	//所有箱子取完

	//	//打开交付口
	//	openSafeDoor();
	//}
	//发送格子号给机械臂,
	int cnum=itBcell->cellNum;
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmTakeEndReport(SOCKET sock, char* str)
{
	SZL_INFO("收到plc发送的机械臂取货完成上报消息");
	RobarmTakeEndReport* pReport = (RobarmTakeEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复机械臂取货完成上报失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmPutEndReport(SOCKET sock, char* str)
{
	SZL_INFO("收到plc发送的机械臂放货完成上报消息");
	RobarmPutEndReport* pReport = (RobarmPutEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复机械臂放货完成上报失败，错误码{0}", WSAGetLastError());
	}
	//箱子推进去
	//goodsBoxIn(11);
	/*成功，出货量减一
	auto itB = m_positionInfoList.begin();
	auto itBcell = itB->vecCellInfo.begin();
	itBcell->outputGoodsNum--;
	if (itBcell->outputGoodsNum > 0)
	{
		robarmTakeGoodsContinue();
	}
	if (itBcell->outputGoodsNum <= 0)
	{
		itB->vecCellInfo.erase(itBcell);
	}
	if (itB->vecCellInfo.size() == 0)
	{
		箱子推进去
		goodsBoxIn(m_BInNum);
		m_positionInfoList.erase(itB);
	}
	if (m_positionInfoList.size() == 0)
	{
		所有箱子取完
		打开交付口
		? 最后一个箱子推进去后再打开交付口还是 同时进行

		openSafeDoor();
	}*/
	
	delete rep;
	rep = nullptr;
}
void TcpServer::customerTakeEndReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的顾客取商品完成上报消息");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的顾客取商品完成上报消息");
	CustomerTakeEndReport* pReport = (CustomerTakeEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	m_isOpenDoor = false;
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复顾客取商品完成上报失败，错误码{0}", WSAGetLastError());
	}
	if (m_IsBoxEx)
	{
		auto itB = m_positionInfoList.begin();
		if (itB != m_positionInfoList.end())
		{
			auto itBcell = itB->vecCellInfo.begin();
			if (itBcell != itB->vecCellInfo.end())
			{
				//下发格子号，位置等信息给机械臂，取货
				SendCellInfoToRobot(itB, itBcell);
			}
		}
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::boxTransforOutReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱转运出库完成上报");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的货箱转运出库完成上报消息");
	BoxTransforOutReport* pReport = (BoxTransforOutReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复货箱转运出库完成上报失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::boxTransforInReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱转运入库完成上报");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的货箱转运入库完成上报消息");
	BoxTransforInReport* pReport = (BoxTransforInReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复货箱转运入库完成上报失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
PlcDevState TcpServer::GetDevState(SOCKET sock)
{
	PlcDevState deState;
	for (int n = (int)_clients.size() - 1; n >= 0; n--)
	{
		if (_clients[n]->sockfd() == sock)
		{
			return _clients[n]->_devState;
		}
	}
	return deState;
}
PlcDevState TcpServer::GetDevState()
{
	PlcDevState deState;
	for (auto& it:_clients)
	{
		if (it->GetSockId() == SockIdType::PLC_CLIENT)
			return it->_devState;
	}
	return deState;
}
bool TcpServer::disConnectClientTimeOut(ClientSocket* pClient)
{
	for (int n = (int)_clients.size() - 1; n >= 0; n--)
	{
		if (_clients[n] == pClient)
		{
			time_t ttm = time(NULL);
			if ((ttm - _clients[n]->getPreHeartTime()) > _intervalBeatConnect)
			{
				SZL_WARN(" heart beat timeout {0}s!!!,客户端<socket={1}>心跳超时已退出，连接断开，任务结束。", _intervalBeatConnect,(int)_clients[n]->sockfd());
				CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
				char tBuf[1024] = { 0 };
				ClientStateReport* state = new ClientStateReport;
				state->_abnormalState = 2;

				sprintf_s(tBuf, " heart beat timeout %ds!!!,客户端<socket=%d>心跳超时已退出，连接断开，任务结束。", _intervalBeatConnect, (int)_clients[n]->sockfd());
				p->NotifyMsg(tBuf);
				closesocket(_clients[n]->sockfd());
				p->SetPlcConnectState(false);
				p->SetPlcStatePlcShow(state);
				m_IsOutGoodsEnd = true;
				m_IsLoadGoodsEnd = true;
				m_productCellInfoList.clear();
				m_positionInfoList.clear();
				m_isOpenDoor = false;
				m_IsBoxEx = false;
				m_IsBoxIn = false;
				m_AddBoxIn = false;
				auto itCl = _clients.begin() + n;
				if (itCl != _clients.end())
				{

					delete _clients[n];
					_clients[n] = nullptr;
					_clients.erase(itCl);
					return true;
				}
				delete state;
				state = nullptr;
			}
			break;
		}
	}
	return false;
}
void TcpServer::goodsBoxOut(char boxNumber,const char* taskNumber)
{
	if (_clients.size() <= 0)
		return;
	ServerGoodsBoxOut* pSend = new ServerGoodsBoxOut;
	pSend->_boxNumber = boxNumber;
	if (taskNumber)
	{
		pSend->_taskNumber[0] = taskNumber[0];
		pSend->_taskNumber[1] = taskNumber[1];
	}
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket={0}>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("货箱出库指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：货箱出库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发货箱出库指令_sendType={0},plcSock=[{1}]", (int)pSend->_sendType, (int)it->sockfd());
	}
}
bool TcpServer::goodsBoxIn(char boxNumber,const char* taskNumber)
{
	if (_clients.size() <= 0)
		return false;
	ServerGoodsBoxIn* pSend = new ServerGoodsBoxIn;
	pSend->_boxNumber = boxNumber;
	if (taskNumber)
	{
		pSend->_taskNumber[0] = taskNumber[0];
		pSend->_taskNumber[1] = taskNumber[1];
	}
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();

		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket={0}>,設備狀態異常或忙碌", (int)it->sockfd());
			return false;
		}
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("货箱入库指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：货箱入库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发货箱入库指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
		return true;
	}
	return false;
}
void TcpServer::robarmTakeGoodsContinue()
{
	if (_clients.size() <= 0)
		return;

	ServerRobarmGoodContinue* pSend = new ServerRobarmGoodContinue;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket={0}>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("机械臂继续取货指令下发失败，错误码{0}", WSAGetLastError());
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		SZL_INFO("服务端下发机械臂继续取货指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::openSafeDoor()
{
	if (_clients.size() <= 0)
		return;

	ServerOpenSafeDoor* pSend = new ServerOpenSafeDoor;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		//打开交付口不用管状态
		/*PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket={0}>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber()+1;
		pSend->_key[1] = ToolHelp::GetRandNumberS()+1;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("打开安全交付门指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：服务端下发打开交付安全门指令****");
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发打开交付安全门指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::goodsBoxTransforOut(char boxNumber)
{
	if (_clients.size() <= 0)
		return;

	ServerGoodsTransforOut* pSend = new ServerGoodsTransforOut;
	pSend->_key[0] = ToolHelp::GetRandNumber();
	pSend->_key[1] = ToolHelp::GetRandNumberS();
	pSend->_boxNumber = boxNumber;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("货箱转运出库指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：货箱转运出库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发货箱转运出库指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::goodsBoxTransforIn(char boxNumber)
{
	if (_clients.size() <= 0)
		return;

	ServerGoodsTransforIn* pSend = new ServerGoodsTransforIn;
	pSend->_boxNumber = boxNumber;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("货箱转运入库指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：货箱转运入库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_ERROR("服务端下发货箱转运入库指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::resetPos()
{
	if (_clients.size() <= 0)
		return;

	ServerResetPos* pSend = new ServerResetPos;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;
	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		/*if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("复位指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：plc复位。****");
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发复位指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::startPlc()
{
	if (_clients.size() <= 0)
		return;

	ServerStartPlc* pSend = new ServerStartPlc;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;

	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		/*PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("启动指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：plc启动。****");
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发启动指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::stopPlc()
{
	if (_clients.size() <= 0)
		return;

	ServerStopPlc* pSend = new ServerStopPlc;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;

	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌,此处是停止plc指令。", (int)it->sockfd());
			//continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("停止指令下发失败，错误码{0}", WSAGetLastError());
			continue;
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：plc停止。****");
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发停止指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::ClientReponse(SOCKET sock, char* str)
{
	SZL_INFO("收到客户端回复指令");
	ClientReponseX* pRep = (ClientReponseX*)str;

	ServerKey key;
	key._key1 = pRep->_key[0];
	key._key2 = pRep->_key[1];
	auto itCmd = _serverSendCmd.find(key);
	if (itCmd == _serverSendCmd.end())
	{
		SZL_ERROR("回复命令未找到！！");
		return;
	}
	//命令执行成功则删除这条下发记录
	if (pRep->_isSuccess == 1)
	{
		ParseReponse(itCmd->second.first);
		if (itCmd != _serverSendCmd.end())
		{
			delete itCmd->second.first;
			itCmd->second.first = nullptr;
			_serverSendCmd.erase(itCmd);
		}
	}
	//失败重发
	else
	{
		if (itCmd != _serverSendCmd.end())
		{
			ServerSend* pSend = (ServerSend*)itCmd->second.first;
			SZL_ERROR("下发命令_sendType=[{0}]执行失败!!!!!!!!!!", (int)pSend->_sendType);
			//RepeatSendCmd(itCmd->second.first);
			delete itCmd->second.first;
			itCmd->second.first = nullptr;
			_serverSendCmd.erase(itCmd);
		}
	}
}
bool TcpServer::RepeatSendCmd(SOCKET csock, DataHeader* str,std::pair<ServerKey,TimeCmd>& pair)
{
	ServerSend* pSend = (ServerSend*)str;
	if (_clients.size() <= 0)
	{
		for (auto &it : _serverSendCmd)
		{
			if (it.second.first)
			{
				delete it.second.first;
				it.second.first = nullptr;
			}
		}
		_serverSendCmd.clear();
		return false;
	}
	PlcDevState st = GetDevState(csock);
	if (st._abnormalState == 2 || st._devState == 2)
	{
		SZL_WARN("<socket={0}>,設備狀態異常或忙碌", (int)csock);
		return false;
	}
	ServerKey key;
	key._key1 = pSend->_key[0];
	key._key2 = pSend->_key[1];

	auto itScmd = _serverSendCmd.find(key);
	if (itScmd == _serverSendCmd.end())
	{
		SZL_ERROR("发送命令未找到，建议砸电脑可解决问题！！");
		return false;
	}
	pSend->_key[0] = ToolHelp::GetRandNumber();
	pSend->_key[1] = ToolHelp::GetRandNumberS();
	if (SOCKET_ERROR == SendData(csock, str))
	{
		SZL_WARN("指令_sendType=[{0}]重发失败，plcSock={1},错误码[{2}]", (int)pSend->_sendType, (int)csock, WSAGetLastError());
	}
	else
	{
		SendRecvTime sendT;
		sendT._timeS= ++itScmd->second.second._timeS;
		sendT.sendTime = time(NULL);
		sendT._sock = csock;
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		pair = std::make_pair(key, tmap);
		SZL_WARN("重发命令类型_sendType=[{0}],plcSock={1}", (int)pSend->_sendType, (int)csock);
	}
	return true;
}
void TcpServer::Routine()
{
	while (1)
	{
		//隔一秒重发一次
		MyThread::SleepMillSec(1000);
		time_t nowT = time(NULL);

		MyThread::Lock();
		if (m_isStopRouine)
		{
			MyThread::UnLock();
			break;
		}
		if (_serverSendCmd.size() <= 0)
		{
			MyThread::UnLock();
			continue;
		}
		auto it = _serverSendCmd.begin();
		while (_serverSendCmd.size() >0 && it != _serverSendCmd.end())
		{
			//重发超过5遍，删除此条命令
			if (it->second.second._timeS >= 5)
			{
				ServerSend* pSend = (ServerSend*)it->second.first;
				SZL_ERROR("命令pSend->_sendType=[{0}],重发超过五遍.plcSock={1}", (int)pSend->_sendType, (int)it->second.second._sock);
				delete it->second.first;
				it->second.first = nullptr;
				_serverSendCmd.erase(it++);
				continue;
			}
			std::pair<ServerKey, TimeCmd> tPair;
			//命令超过一秒未收到回复，重发一次
			if (nowT - it->second.second.sendTime > 1 && it->second.second._timeS <= 5)
			{
				if (!RepeatSendCmd(it->second.second._sock, it->second.first, tPair))
				{
					break;
				}
				_serverSendCmd.erase(it++);
				_serverSendCmd.insert(tPair);
				continue;
			}
			it++;
		}
		MyThread::UnLock();
	}
}
bool TcpServer::ContinueOutBox()
{
	if (m_productCellInfoList.size() <= 0)
		return false;
	SZL_INFO("上货继续推出货箱!!!");
	auto itB = m_productCellInfoList.begin();
	/*char tBuf[4096] = { 0 };
	for (auto &it : itB->vecCellInfo)
	{
		if (it.cellLoadQ <= 0)
			continue;
		char buf[256] = { 0 };
		sprintf_s(buf, "请在推出的货箱的第%d个格子中,放入%d瓶%s\n", it.cellNum, it.cellLoadQ, it.productName.c_str());
		strcat(tBuf, buf);
	}
	WarningDlg dlg(nullptr, tBuf);
	dlg.DoModal();*/
	std::string cId = itB->container.containerID;

	ContainerInfoPLC cPlc;
	UShop::PLCDB::Instance().QueryContainerInfoByID(cId, cPlc);

	AddGoodsBoxOut(std::stoi(cPlc.shelfID));
	/*ShowAddGoodsInfo GoodsInfodlg(nullptr, m_productCellInfoList, 1, *itB);
	GoodsInfodlg.DoModal();*/
	//设置当前推出的货架号
	m_curBoxNum = std::stoi(cPlc.shelfID);
	//更新数据库
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	p->UpdateInGoodsInfo(cId, m_productCellInfoList);
	p->ShowLoadInfoForAPP(*itB);
	m_productCellInfoList.erase(itB);
	return true;
}
void TcpServer::ParseReponse(DataHeader* head)
{
	ServerSend* pSend = (ServerSend*)head;
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOOD_BOX_IN)
	{
		SZL_INFO("收到plc发送的货箱入库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::ADDGOODS_BOXIN)
	{
		SZL_INFO("收到plc发送的补货入库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::ADDGOODS_BOXOUT)
	{
		SZL_INFO("收到plc发送的补货出库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_OUT)
	{
		SZL_INFO("收到plc发送的货箱出库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_TRANSFOR_IN)
	{
		SZL_INFO("收到plc发送的货箱转运入库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_TRANSFOR_OUT)
	{
		SZL_INFO("收到plc发送的货箱转运出库指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::OPEN_SAFE_DOOR)
	{
		SZL_INFO("收到plc发送的打开安全交付口指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::RESET_POS)
	{
		SZL_INFO("收到plc发送的复位指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::SRART_PLC)
	{
		SZL_INFO("收到plc发送的启动指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::STOP_PLC)
	{
		SZL_INFO("收到plc发送的停止指令回复");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::WARN_RESET)
	{
		SZL_INFO("收到plc发送的报警复位指令回复");
	}
}
void TcpServer::SendCellInfoToRobot(std::vector<CompleteBox>::iterator itB, std::vector<CellInfoPLC>::iterator itBcell)
{
	if (!GetArmMachineOptState())
	{
		SZL_ERROR("当前机器臂存在抓取操作异常，请点击[启动]按钮进行重置");
		return;
	}
	//下发格子号，位置等信息给机械臂，取货

	if (!m_isOpenDoor)
	{
		//货箱类型,1 -> 2X3 or 3X2货箱，2 -> 3X3型货箱
		std::string strBoxType = itB->container.containerType;
		int boxType = 0;
		if (strBoxType == "2x3")
			boxType = 1;
		else if (strBoxType == "3x3")
			boxType = 2;
		//货箱编号
		int boxId = std::stoi(itB->container.containerID.substr(4, itB->container.containerID.size()-4));
		//位置1-18(左)，19-36(右)
		int boxPos = boxId <= 18 ? 0 : 1;
		//格子号
		int cellId = itBcell->cellNum;
		//格子库存
		int cellStock = itBcell->cellStock;
		//商品条码
		std::string barCode = itBcell->barcode;
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "下发出货格子号等信息给机械臂。货箱类型：%d,货箱编号:%d,格子号:%d,商品条码:%s,商品库存:%d", boxType, boxId, cellId, barCode.c_str(), cellStock);
		p->NotifyMsg(tBuf);
		SZL_INFO("下发出货格子号等信息。货箱类型：{0},货箱编号:{1},格子号:{2},商品条码:{3},商品库存:{4}", boxType, boxId, cellId, barCode, cellStock);
		if (cellId < 1|| cellStock<1)
		{
			SZL_ERROR("格子号或者对应库存错误。");
			return;
		}
		ProductInfoPLC goodsInfo;
		UShop::PLCDB::Instance().QueryProductInfoByBarcode(barCode.c_str(),goodsInfo);
		//将局部变量传递给私有成员，防止lambada引用传值局部变量在外部被提前释放
		m_pickUpGoodsService.setCheckOutGoodsPara(boxPos, cellId, boxType, cellStock, goodsInfo);
		std::thread t1([&]() {

			//m_pickUpGoodsService.checkOutGoods(boxPos, cellId, boxType, barCode.c_str());
			//m_pickUpGoodsService.checkOutGoods(boxPos, cellId, boxType, cellStock,barCode.c_str());
			if (!m_pickUpGoodsService.checkOutGoods())
			{
				SetArmMachineOptState(false);
			}
		});
		t1.detach();

	}
}
bool TcpServer::ContOutBox()
{
	if (m_positionInfoList.size() <= 0)
	{
		SZL_INFO("出货货箱数量信息：{0}", m_positionInfoList.size());
		return false;
	}
	SZL_INFO("出货货箱数量信息：{0}", m_positionInfoList.size());
	SZL_INFO("出货继续推出货箱！！");
	auto itB = m_positionInfoList.begin();

	ContainerInfoPLC cPlc;
	UShop::PLCDB::Instance().QueryContainerInfoByID(itB->container.containerID, cPlc);

	goodsBoxOut(std::stoi(cPlc.shelfID));
	m_BInNum = std::stoi(cPlc.shelfID);
	return true;
}
void TcpServer::AddGoodsBoxOut(char boxNumber)
{
	if (_clients.size() <= 0)
		return;

	ServerAddGoodsBoxout* pSend = new ServerAddGoodsBoxout;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;

	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("补货出库指令下发失败，错误码{0}", WSAGetLastError());
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：补货出库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发补货出库指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::AddGoodsBoxIn(char boxNumber)
{
	if (_clients.size() <= 0)
		return;

	ServerAddGoodsBoxin* pSend = new ServerAddGoodsBoxin;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;

	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("补货入库指令下发失败，错误码{0}", WSAGetLastError());
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********服务端下发指令：补货入库，货架号[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发补货入库指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::AddGoodsBoxOutReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱补货出库完成上报");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的补货出库完成上报消息");
	ServerAddGoodsBoxout* pReport = (ServerAddGoodsBoxout*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复补货出库完成上报失败，错误码{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::AddGoodsBoxInReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "收到plc发送的货箱补货入库完成上报");
	p->NotifyMsg(tBuf);
	SZL_INFO("收到plc发送的补货入库完成上报消息");
	ServerAddGoodsBoxin* pReport = (ServerAddGoodsBoxin*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("回复补货入库完成上报失败，错误码{0}", WSAGetLastError());
	}
	m_AddBoxIn = true;
	delete rep;
	rep = nullptr;
}
void TcpServer::XYZPosReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	ClientXYZPosReport* pReport = (ClientXYZPosReport*)str;
	char buf[256] = { 0 };
	FLAOT_UNION xpos, ypos, zpos;
	//字节转化为浮点数
	xpos.float_byte.high_byte = pReport->_xPos[0];
	xpos.float_byte.mhigh_byte= pReport->_xPos[1];
	xpos.float_byte.mlow_byte = pReport->_xPos[2];
	xpos.float_byte.low_byte  = pReport->_xPos[3];

	ypos.float_byte.high_byte = pReport->_yPos[0];
	ypos.float_byte.mhigh_byte= pReport->_yPos[1];
	ypos.float_byte.mlow_byte = pReport->_yPos[2];
	ypos.float_byte.low_byte  = pReport->_yPos[3];

	zpos.float_byte.high_byte = pReport->_zPos[0];
	zpos.float_byte.mhigh_byte= pReport->_zPos[1];
	zpos.float_byte.mlow_byte = pReport->_zPos[2];
	zpos.float_byte.low_byte  = pReport->_zPos[3];

	sprintf_s(buf,"货箱位置: X:%.2f,Y:%.2f,Z:%.2f", xpos.value, ypos.value, zpos.value);
	p->BoxPosUpdate(buf);
}
void TcpServer::RobotArmPutGoodsEnd(bool bSucess)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	if (!bSucess)
	{
		SZL_WARN("机械臂放货失败.##########。");
		sprintf_s(tBuf, "**********机械臂放货失败********。");
	}
	else
	{
		sprintf_s(tBuf, "**********机械臂放货成功********。");
		SZL_INFO("机械臂放货完成。");
	}
	p->NotifyMsg(tBuf);
	m_MaxTakeNum++;
	if (m_positionInfoList.size() <= 0)
		return;
	SZL_INFO("出貨貨箱數量:{0}", m_positionInfoList.size());
	auto itB = m_positionInfoList.begin();
	auto itBcell = itB->vecCellInfo.begin();
	SZL_INFO("格子号：{0},对应要出货数量{1}", itBcell->cellNum, itBcell->outputGoodsNum);
	//成功，出货量减一
	itBcell->outputGoodsNum--;
	itBcell->cellStock--;
	SZL_INFO("格子号：{0},对应要出货数量{1}", itBcell->cellNum,itBcell->outputGoodsNum);
	//出货口商品数量大于5，打开交付口
	if (m_MaxTakeNum >= 5)
	{
		//打开交付口
		memset(tBuf, 0X00, sizeof(tBuf));
		sprintf_s(tBuf, "************出货口商品数量大于5，打开交付口***********");
		p->NotifyMsg(tBuf);
		SZL_INFO("************出货口商品数量大于5，打开交付口***********");
		openSafeDoor();
		m_MaxTakeNum = 0;
		m_isOpenDoor = true;
	}
	if (itBcell->outputGoodsNum > 0)
	{
		//下发格子号，位置等信息给机械臂，取货
		SendCellInfoToRobot(itB, itBcell);
	}
	if (itBcell->outputGoodsNum <= 0)
	{
		itB->vecCellInfo.erase(itBcell);
		if (itB->vecCellInfo.size() > 0)
		{
			itBcell = itB->vecCellInfo.begin();
			if (itBcell->outputGoodsNum > 0)
			{
				//下发格子号，位置等信息给机械臂，取货
				SendCellInfoToRobot(itB, itBcell);
			}
			if (itBcell->outputGoodsNum <= 0)
			{
				itB->vecCellInfo.erase(itBcell);
			}
		}
	}
	if (itB->vecCellInfo.size() == 0)
	{
		SZL_INFO("出貨貨箱數量:{0}", m_positionInfoList.size());
		m_positionInfoList.erase(itB);
		SZL_INFO("出貨貨箱數量:{0}", m_positionInfoList.size());
		//箱子推进去
		goodsBoxIn(m_BInNum);
		m_IsBoxEx = false;
	}
	if (m_positionInfoList.size() == 0)
	{
		//所有箱子取完
		//打开交付口
		memset(tBuf, 0X00, sizeof(tBuf));
		sprintf_s(tBuf,"***************所有箱子取完，打开交付口*************");
		p->NotifyMsg(tBuf);
		SZL_INFO("***************所有箱子取完，打开交付口*************");
		if (!m_isOpenDoor)
		{
			openSafeDoor();
			m_MaxTakeNum = 0;
			m_isOpenDoor = true;
		}
	}
}
void TcpServer::RobotArmTakeGoodsEnd()
{

}
BOOL TcpServer::Open(LPCTSTR profile, LPCTSTR section)
{
	CProfile file;
	file.SetProfile(profile);

	_port = file.GetLong(section, _T("Port"), _T("9000"));
	_port2 = file.GetLong(section, _T("Port2"), _T("9001"));
	_ip = file.GetString(section, _T("IP"), _T("0"));
	_intervalBeatConnect = file.GetLong(section, _T("BeatConnect"), _T("5000"));
	SZL_INFO("配置文件读取到的端口号1:{0},端口号2:{1},ip地址:{2},心跳断连时间间隔:{3}s", _port,_port2,_ip, _intervalBeatConnect);
	return TRUE;
}
void TcpServer::WarningHandle(const char* buf)
{
	//CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	m_plcWarning.DealWarning1(buf[0], m_PlcDlg);
	m_plcWarning.DealWarning2(buf[1], m_PlcDlg);
	m_plcWarning.DealWarning3(buf[2], m_PlcDlg);
	m_plcWarning.DealWarning4(buf[3], m_PlcDlg);
	m_plcWarning.DealWarning5(buf[4], m_PlcDlg);
	m_plcWarning.DealWarning6(buf[5], m_PlcDlg);
}
void TcpServer::PlcWarnReset()
{
	if (_clients.size() <= 0)
		return;

	ServerWarnReset* pSend = new ServerWarnReset;
	pSend->_devId = _devInfo._devId;
	pSend->_devType = _devInfo._devType;

	for (auto &it : _clients)
	{
		if (it->GetSockId() != SockIdType::PLC_CLIENT)
			continue;
		/*PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket=%d>,設備狀態異常或忙碌", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		//pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("报警复位指令下发失败，错误码{0}", WSAGetLastError());
		}
		SendRecvTime sendT;
		sendT._timeS++;
		sendT.sendTime = time(NULL);
		sendT._sock = it->sockfd();
		TimeCmd tmap(pSend, sendT);
		ServerKey key;
		key._key1 = pSend->_key[0];
		key._key2 = pSend->_key[1];
		_serverSendCmd.insert(std::make_pair(key, tmap));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "******** 服务端下发指令：报警复位 ****");
		p->NotifyMsg(tBuf);
		SZL_INFO("服务端下发报警复位指令_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}