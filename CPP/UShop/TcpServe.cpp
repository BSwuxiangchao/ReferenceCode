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
	//�ر��׽���
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

	//�ر�windows socket����
	WSACleanup();
	_clients.clear();
}
//��ʼ��socket
SOCKET TcpServer::IntiSocket()
{
	//����win Sock 2.x����
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
		SZL_INFO("<socket={0}>,<socket2={1}>�رվ�����", (int)_sock, (int)_sock2);
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		SZL_ERROR("����socketʧ��");
	}
	else
	{
		SZL_INFO("����<socket={0}�ɹ�.", (int)_sock);
	}
	_sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock2)
	{
		SZL_ERROR("����socketʧ��");
	}
	else
	{
		SZL_INFO("����<socket={0}�ɹ�.", (int)_sock2);
	}
	return _sock;
}
//�󶨶˿ں�
int TcpServer::Bind()
{
	//bind �����ڽ��տͻ������ӵ�����˿�
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
		SZL_ERROR("������˿�<{0}>ʧ��,������[{1}]", (int)_port, GetLastError());
	}
	else
	{
		SZL_INFO("������˿�<{0}>�ɹ�", (int)_port);
	}
	_sin.sin_port = htons(_port2);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("������˿�<{0}>ʧ��,������[{1}]", (int)_port2, GetLastError());
	}
	else
	{
		SZL_INFO("������˿�<{0}>�ɹ�", (int)_port2);
	}
	ret= ::bind(_sock2, (sockaddr*)&_sin, sizeof(sockaddr_in));
	return ret;
}
//��������˿�
int TcpServer::Listen(int n)
{
	int ret = listen(_sock, n);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("<socket={0}>���󣬼�������˿�ʧ��", (int)_sock);
	}
	else
	{
		SZL_INFO("<socket={0}>�ɹ�����������˿ڳɹ�", (int)_sock);
	}
	ret = listen(_sock2, n);
	if (ret == SOCKET_ERROR)
	{
		SZL_ERROR("<socket={0}>���󣬼�������˿�ʧ��", (int)_sock2);
	}
	else
	{
		SZL_INFO("<socket={0}>�ɹ�����������˿ڳɹ�", (int)_sock2);
	}
	return ret;
}
SOCKET TcpServer::Accept()
{
	//accept�ȴ����տͻ�������
	sockaddr_in ClientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(_sock, (sockaddr*)&ClientAddr, &nAddrLen);
	if (cSock == INVALID_SOCKET)
	{
		SZL_ERROR("<socket={0}>���󣬽�����Ч�ͻ���SOCKET", (int)_sock);
	}
	else
	{
		ClientSocket* cSoc = new ClientSocket(cSock);
		cSoc->SetSockId(SockIdType::PLC_CLIENT);
		_clients.push_back(cSoc);
		SZL_INFO("<socket={0}>�¿ͻ��˼��룺socket={1},IP={2}", (int)_sock, (int)cSock, inet_ntoa(ClientAddr.sin_addr));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		p->SetPlcConnectState(true);
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "plc�ͻ������ӳɹ���IP:%s,�˿�:%d", inet_ntoa(ClientAddr.sin_addr), ClientAddr.sin_port);
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
	//accept�ȴ����տͻ�������
	sockaddr_in ClientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(_sock2, (sockaddr*)&ClientAddr, &nAddrLen);
	if (cSock == INVALID_SOCKET)
	{
		SZL_ERROR("<socket={0}>���󣬽�����Ч�ͻ���SOCKET", (int)_sock2);
	}
	else
	{
		ClientSocket* cSoc = new ClientSocket(cSock);
		cSoc->SetSockId(SockIdType::POS_CLIENT);
		_clients.push_back(cSoc);
		SZL_INFO("<socket={0}>�¿ͻ������ӣ�socket={1},IP={2}", (int)_sock2, (int)cSock, inet_ntoa(ClientAddr.sin_addr));
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		//p->SetPlcConnectState(true);
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "plcλ���ϱ�ר�ö˿����ӳɹ���IP:%s,�˿�:%d", inet_ntoa(ClientAddr.sin_addr), ClientAddr.sin_port);
		p->NotifyMsg(tBuf);
	}
	return cSock;
}
bool TcpServer::OnRun()
{
	if (isRun())
	{
		//�������׽��� BSD socket
		//����������
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//������
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//���뼯��
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
		//nfds��һ������ֵ����ָfd_set���ϵ������������ķ�Χ������������
		//���������ļ����������ֵ+1����windows��������0
		timeval t = { 1,0 }; //���ó�ʱʱ��1��
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			SZL_INFO("select�������");
			Close();
			return false;
		}
		//�ж��������Ƿ��ڼ�����
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
		//�ж��������Ƿ��ڼ�����
		if (FD_ISSET(_sock2, &fdRead))
		{
			FD_CLR(_sock2, &fdRead);
			Accept2();
		}
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			//��������30�룬�߳��ͻ���
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
	//�յ�����������������ʱ��
	time_t ttm = time(NULL);
	pClient->setHeartTime(ttm);
	//���տͻ�������
	memset(_szRecv, 0x00, sizeof(_szRecv));
	int nLen = (int)recv(pClient->sockfd(), _szRecv, RECV_BUFF_SIZE, 0);
	//�����յ������ݿ�������Ϣ������
	if (nLen <= 0)
	{
		SZL_INFO("�ͻ���<socket={0}>���˳����������", (int)pClient->sockfd());
		return -1;
	}
	std::string sStr = ToolHelp::GetHexString(_szRecv, nLen);
	SZL_DEBUG("�ͻ���<socket={0}>���͵���Ϣ:{1}", (int)pClient->sockfd(), sStr.c_str());
	memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);
	//��Ϣ��������β������λ�ú���
	pClient->setLastPos(pClient->getLastPos() + nLen);
	//�ж���Ϣ�����������ݳ��ȴ���DataHeader��Ϣͷ����
	while (pClient->getLastPos() >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)pClient->msgBuf();
		//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		if (pClient->getLastPos() >= header->_len)
		{
			//ʣ��δ������Ϣ���������ݵĳ���
			int nSize = pClient->getLastPos() - header->_len;
			//����������Ϣ
			OnNetMsg(pClient->sockfd(), pClient->msgBuf());
			//����Ϣ������ʣ������ǰ��
			memcpy(pClient->msgBuf(), pClient->msgBuf() + header->_len, nSize);
			//��Ϣ������β������λ��ǰ��
			pClient->setLastPos(nSize);
		}
		else
		{
			//��Ϣ�������������ݲ���һ��������Ϣ
			break;
		}
	}
	return 0;
}
void TcpServer::OnNetMsg(SOCKET cSock, char* str)
{
	if ((std::string)str == "")
	{
		SZL_WARN("���յ����ַ���Ϊ��");
		return;
	}
	DataHeader* header = (DataHeader*)str;
	SZL_DEBUG("�ͻ��˷��͵���Ϣ���� _msgType={0}", (int)header->_msgType);
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
		SZL_INFO("<socket={0}>�յ�δ������Ϣ�����ݳ��ȣ�{1}", (int)cSock, (int)header->_len);
	}
	break;
	}
}
int TcpServer::SendData(SOCKET cSock, DataHeader* header)
{
	if (isRun() && header)
	{
		//��plc�������ݳ��ȹ̶�Ϊ12
		std::string sStr = ToolHelp::GetHexString((char*)header, 12);
		SZL_INFO("����˷��͵����ݣ�{0}", sStr.c_str());
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
	SZL_DEBUG("�ͻ����ϱ����ͣ�_reportType={0}", (int)report->_reportType);
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
		SZL_WARN("�ͻ����ϱ�����:{0} δ���壡������", (int)report->_reportType);
	}
	break;
	}
}
void TcpServer::heartBeat(SOCKET cSock, char* str)
{
	SZL_INFO("�յ�plc���͵�������Ϣ");

	ClientHeartBeat* beat = (ClientHeartBeat*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::HEART_BEAT_REP;
	memcpy(rep->_key, beat->_key, sizeof(beat->_key));
	rep->_devType = beat->_devType;
	rep->_devId = beat->_devId;

	if (SOCKET_ERROR == SendData(cSock, rep))
	{
		SZL_ERROR("�ظ�����ʧ�ܣ�������{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::Login(SOCKET cSock, char* str)
{
	SZL_INFO("�յ�plc���͵ĵ�¼��Ϣ");

	ClientLogin* login = (ClientLogin*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::LOGIN_REP;
	memcpy(rep->_key, login->_key, sizeof(login->_key));
	rep->_devType = login->_devType;
	rep->_devId = login->_devId;
	_devInfo._devId = login->_devId;
	_devInfo._devType = login->_devType;
	//������ݱ�ʶ
	for (auto &it : _clients)
	{
		if (cSock == it->sockfd())
		{
			it->SetSockId(SockIdType::PLC_CLIENT);
		}
	}
	if (SOCKET_ERROR == SendData(cSock, rep))
	{
		SZL_ERROR("�ظ���¼ʧ�ܣ�������{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::stateReport(SOCKET sock, char* str)
{
	SZL_INFO("�յ�plc���͵��豸״̬�ϱ���Ϣ");

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
			char _controlMode;				 //����ģʽ,1���Զ�ģʽ��2���ֶ�ģʽ
			char _devState;					 //�豸״̬,1�����У�2��æµ
			char _abnormalState;			 //�쳣״̬ 1������ 2���쳣
			*/
			CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
			p->SetPlcStatePlcShow(pReport);
			char tBuf[1024] = { 0 };
			sprintf_s(tBuf, "�յ�plc�豸״̬�ϱ�,����ģʽ:[%d],�豸״̬:[%d],�쳣״̬:[%d]", (int)pReport->_controlMode, (int)pReport->_devState, (int)pReport->_abnormalState);
			p->NotifyMsg(tBuf);
			std::string str= pReport->_warningMsg;
			WarningHandle(pReport->_warningMsg);
			break;
		}
	}
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ��豸״̬�ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	if (m_IsBoxIn)
	{
		if (!m_isBtnBox)
		{
			bool ret = ContOutBox();
			if (!ret)
			{
				m_IsOutGoodsEnd = true;
				SZL_INFO("���г�����ɣ��������ݿ�******************��");
				CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
				//�������ݿ�
				p->UpdateOutGoodsInfo();
				p->NotifyMsg("���г�����ɣ��������ݿ�******************��");
				//���͸���Ʒ�����Ϣ���ܿ�
				p->UpdateStockToServer(2);
			}
		}
		m_isBtnBox = false;
		m_IsBoxIn = false;
	}
	if (m_AddBoxIn)
	{
		//�����Ƴ����Ӳ���
		if (/*!m_IsAIstoreMode*/1)
		{
			if (!m_isBtnBox)
			{
				bool ret = ContinueOutBox();
				if (!ret && !m_isBtnBox)
				{
					CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
					p->NotifyMsg("���в�����ɣ���");
					SZL_INFO("���в�����ɣ���");
					SZL_INFO("���¿�浽��̨����");
					p->UpdateInGoodsInfoToBg(0);
					//���͸���Ʒ�����Ϣ���ܿ�
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
	sprintf_s(tBuf, "�յ�plc���͵Ļ����������ϱ���Ϣ");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ļ����������ϱ���Ϣ");
	ClientBoxOutReport* pReport = (ClientBoxOutReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	int len = SendData(sock, rep);
	if (SOCKET_ERROR == len)
	{
		SZL_ERROR("�ظ������������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	m_IsBoxEx = true;
	auto itB = m_positionInfoList.begin();
	if (itB != m_positionInfoList.end())
	{
		auto itBcell = itB->vecCellInfo.begin();
		if (itBcell != itB->vecCellInfo.end())
		{
			//�·����Ӻţ�λ�õ���Ϣ����е�ۣ�ȡ��
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
	sprintf_s(tBuf, "�յ�plc���͵Ļ����������ϱ���Ϣ");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ļ����������ϱ���Ϣ");
	ClientBoxInReport* pReport = (ClientBoxInReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ������������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	m_IsBoxIn = true;
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmTakeReport(SOCKET sock, char* str)
{
	SZL_INFO("�յ�plc���͵Ļ�е������ȡ���ϱ���Ϣ");
	RobarmTakeReport* pReport = (RobarmTakeReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ���е������ȡ���ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
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
	//	//�����ƽ�ȥ
	//	goodsBoxIn(m_BInNum);
	//	m_positionInfoList.erase(itB);
	//}
	//if (m_positionInfoList.size() == 0)
	//{
	//	//��������ȡ��

	//	//�򿪽�����
	//	openSafeDoor();
	//}
	//���͸��ӺŸ���е��,
	int cnum=itBcell->cellNum;
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmTakeEndReport(SOCKET sock, char* str)
{
	SZL_INFO("�յ�plc���͵Ļ�е��ȡ������ϱ���Ϣ");
	RobarmTakeEndReport* pReport = (RobarmTakeEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ���е��ȡ������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::robarmPutEndReport(SOCKET sock, char* str)
{
	SZL_INFO("�յ�plc���͵Ļ�е�۷Ż�����ϱ���Ϣ");
	RobarmPutEndReport* pReport = (RobarmPutEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ���е�۷Ż�����ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	//�����ƽ�ȥ
	//goodsBoxIn(11);
	/*�ɹ�����������һ
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
		�����ƽ�ȥ
		goodsBoxIn(m_BInNum);
		m_positionInfoList.erase(itB);
	}
	if (m_positionInfoList.size() == 0)
	{
		��������ȡ��
		�򿪽�����
		? ���һ�������ƽ�ȥ���ٴ򿪽����ڻ��� ͬʱ����

		openSafeDoor();
	}*/
	
	delete rep;
	rep = nullptr;
}
void TcpServer::customerTakeEndReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "�յ�plc���͵Ĺ˿�ȡ��Ʒ����ϱ���Ϣ");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ĺ˿�ȡ��Ʒ����ϱ���Ϣ");
	CustomerTakeEndReport* pReport = (CustomerTakeEndReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;
	m_isOpenDoor = false;
	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ��˿�ȡ��Ʒ����ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	if (m_IsBoxEx)
	{
		auto itB = m_positionInfoList.begin();
		if (itB != m_positionInfoList.end())
		{
			auto itBcell = itB->vecCellInfo.begin();
			if (itBcell != itB->vecCellInfo.end())
			{
				//�·����Ӻţ�λ�õ���Ϣ����е�ۣ�ȡ��
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
	sprintf_s(tBuf, "�յ�plc���͵Ļ���ת�˳�������ϱ�");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ļ���ת�˳�������ϱ���Ϣ");
	BoxTransforOutReport* pReport = (BoxTransforOutReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ�����ת�˳�������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::boxTransforInReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "�յ�plc���͵Ļ���ת���������ϱ�");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ļ���ת���������ϱ���Ϣ");
	BoxTransforInReport* pReport = (BoxTransforInReport*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ�����ת���������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
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
				SZL_WARN(" heart beat timeout {0}s!!!,�ͻ���<socket={1}>������ʱ���˳������ӶϿ������������", _intervalBeatConnect,(int)_clients[n]->sockfd());
				CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
				char tBuf[1024] = { 0 };
				ClientStateReport* state = new ClientStateReport;
				state->_abnormalState = 2;

				sprintf_s(tBuf, " heart beat timeout %ds!!!,�ͻ���<socket=%d>������ʱ���˳������ӶϿ������������", _intervalBeatConnect, (int)_clients[n]->sockfd());
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
			SZL_ERROR("<socket={0}>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("�������ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�������⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("������·��������ָ��_sendType={0},plcSock=[{1}]", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket={0}>,�O���B������æµ", (int)it->sockfd());
			return false;
		}
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("�������ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
			continue;
		}
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "********������·�ָ�������⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("������·��������ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket={0}>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("��е�ۼ���ȡ��ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		SZL_INFO("������·���е�ۼ���ȡ��ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
		//�򿪽����ڲ��ù�״̬
		/*PlcDevState st = GetDevState(it->sockfd());
		if (st._abnormalState == 2 || st._devState == 2)
		{
			SZL_ERROR("<socket={0}>,�O���B������æµ", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber()+1;
		pSend->_key[1] = ToolHelp::GetRandNumberS()+1;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("�򿪰�ȫ������ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�������·��򿪽�����ȫ��ָ��****");
		p->NotifyMsg(tBuf);
		SZL_INFO("������·��򿪽�����ȫ��ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("����ת�˳���ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�����ת�˳��⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("������·�����ת�˳���ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("����ת�����ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�����ת����⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_ERROR("������·�����ת�����ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("��λָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�plc��λ��****");
		p->NotifyMsg(tBuf);
		SZL_INFO("������·���λָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("����ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�plc������****");
		p->NotifyMsg(tBuf);
		SZL_INFO("������·�����ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ,�˴���ֹͣplcָ�", (int)it->sockfd());
			//continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("ָֹͣ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�plcֹͣ��****");
		p->NotifyMsg(tBuf);
		SZL_INFO("������·�ָֹͣ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::ClientReponse(SOCKET sock, char* str)
{
	SZL_INFO("�յ��ͻ��˻ظ�ָ��");
	ClientReponseX* pRep = (ClientReponseX*)str;

	ServerKey key;
	key._key1 = pRep->_key[0];
	key._key2 = pRep->_key[1];
	auto itCmd = _serverSendCmd.find(key);
	if (itCmd == _serverSendCmd.end())
	{
		SZL_ERROR("�ظ�����δ�ҵ�����");
		return;
	}
	//����ִ�гɹ���ɾ�������·���¼
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
	//ʧ���ط�
	else
	{
		if (itCmd != _serverSendCmd.end())
		{
			ServerSend* pSend = (ServerSend*)itCmd->second.first;
			SZL_ERROR("�·�����_sendType=[{0}]ִ��ʧ��!!!!!!!!!!", (int)pSend->_sendType);
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
		SZL_WARN("<socket={0}>,�O���B������æµ", (int)csock);
		return false;
	}
	ServerKey key;
	key._key1 = pSend->_key[0];
	key._key2 = pSend->_key[1];

	auto itScmd = _serverSendCmd.find(key);
	if (itScmd == _serverSendCmd.end())
	{
		SZL_ERROR("��������δ�ҵ��������ҵ��Կɽ�����⣡��");
		return false;
	}
	pSend->_key[0] = ToolHelp::GetRandNumber();
	pSend->_key[1] = ToolHelp::GetRandNumberS();
	if (SOCKET_ERROR == SendData(csock, str))
	{
		SZL_WARN("ָ��_sendType=[{0}]�ط�ʧ�ܣ�plcSock={1},������[{2}]", (int)pSend->_sendType, (int)csock, WSAGetLastError());
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
		SZL_WARN("�ط���������_sendType=[{0}],plcSock={1}", (int)pSend->_sendType, (int)csock);
	}
	return true;
}
void TcpServer::Routine()
{
	while (1)
	{
		//��һ���ط�һ��
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
			//�ط�����5�飬ɾ����������
			if (it->second.second._timeS >= 5)
			{
				ServerSend* pSend = (ServerSend*)it->second.first;
				SZL_ERROR("����pSend->_sendType=[{0}],�ط��������.plcSock={1}", (int)pSend->_sendType, (int)it->second.second._sock);
				delete it->second.first;
				it->second.first = nullptr;
				_serverSendCmd.erase(it++);
				continue;
			}
			std::pair<ServerKey, TimeCmd> tPair;
			//�����һ��δ�յ��ظ����ط�һ��
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
	SZL_INFO("�ϻ������Ƴ�����!!!");
	auto itB = m_productCellInfoList.begin();
	/*char tBuf[4096] = { 0 };
	for (auto &it : itB->vecCellInfo)
	{
		if (it.cellLoadQ <= 0)
			continue;
		char buf[256] = { 0 };
		sprintf_s(buf, "�����Ƴ��Ļ���ĵ�%d��������,����%dƿ%s\n", it.cellNum, it.cellLoadQ, it.productName.c_str());
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
	//���õ�ǰ�Ƴ��Ļ��ܺ�
	m_curBoxNum = std::stoi(cPlc.shelfID);
	//�������ݿ�
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
		SZL_INFO("�յ�plc���͵Ļ������ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::ADDGOODS_BOXIN)
	{
		SZL_INFO("�յ�plc���͵Ĳ������ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::ADDGOODS_BOXOUT)
	{
		SZL_INFO("�յ�plc���͵Ĳ�������ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_OUT)
	{
		SZL_INFO("�յ�plc���͵Ļ������ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_TRANSFOR_IN)
	{
		SZL_INFO("�յ�plc���͵Ļ���ת�����ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::GOODS_BOX_TRANSFOR_OUT)
	{
		SZL_INFO("�յ�plc���͵Ļ���ת�˳���ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::OPEN_SAFE_DOOR)
	{
		SZL_INFO("�յ�plc���͵Ĵ򿪰�ȫ������ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::RESET_POS)
	{
		SZL_INFO("�յ�plc���͵ĸ�λָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::SRART_PLC)
	{
		SZL_INFO("�յ�plc���͵�����ָ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::STOP_PLC)
	{
		SZL_INFO("�յ�plc���͵�ָֹͣ��ظ�");
	}
	if ((int)pSend->_sendType == (int)SERVER_PLC_MSG_TYPE_CMD::WARN_RESET)
	{
		SZL_INFO("�յ�plc���͵ı�����λָ��ظ�");
	}
}
void TcpServer::SendCellInfoToRobot(std::vector<CompleteBox>::iterator itB, std::vector<CellInfoPLC>::iterator itBcell)
{
	if (!GetArmMachineOptState())
	{
		SZL_ERROR("��ǰ�����۴���ץȡ�����쳣������[����]��ť��������");
		return;
	}
	//�·����Ӻţ�λ�õ���Ϣ����е�ۣ�ȡ��

	if (!m_isOpenDoor)
	{
		//��������,1 -> 2X3 or 3X2���䣬2 -> 3X3�ͻ���
		std::string strBoxType = itB->container.containerType;
		int boxType = 0;
		if (strBoxType == "2x3")
			boxType = 1;
		else if (strBoxType == "3x3")
			boxType = 2;
		//������
		int boxId = std::stoi(itB->container.containerID.substr(4, itB->container.containerID.size()-4));
		//λ��1-18(��)��19-36(��)
		int boxPos = boxId <= 18 ? 0 : 1;
		//���Ӻ�
		int cellId = itBcell->cellNum;
		//���ӿ��
		int cellStock = itBcell->cellStock;
		//��Ʒ����
		std::string barCode = itBcell->barcode;
		CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
		char tBuf[1024] = { 0 };
		sprintf_s(tBuf, "�·��������Ӻŵ���Ϣ����е�ۡ��������ͣ�%d,������:%d,���Ӻ�:%d,��Ʒ����:%s,��Ʒ���:%d", boxType, boxId, cellId, barCode.c_str(), cellStock);
		p->NotifyMsg(tBuf);
		SZL_INFO("�·��������Ӻŵ���Ϣ���������ͣ�{0},������:{1},���Ӻ�:{2},��Ʒ����:{3},��Ʒ���:{4}", boxType, boxId, cellId, barCode, cellStock);
		if (cellId < 1|| cellStock<1)
		{
			SZL_ERROR("���ӺŻ��߶�Ӧ������");
			return;
		}
		ProductInfoPLC goodsInfo;
		UShop::PLCDB::Instance().QueryProductInfoByBarcode(barCode.c_str(),goodsInfo);
		//���ֲ��������ݸ�˽�г�Ա����ֹlambada���ô�ֵ�ֲ��������ⲿ����ǰ�ͷ�
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
		SZL_INFO("��������������Ϣ��{0}", m_positionInfoList.size());
		return false;
	}
	SZL_INFO("��������������Ϣ��{0}", m_positionInfoList.size());
	SZL_INFO("���������Ƴ����䣡��");
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("��������ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ��������⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("������·���������ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("�������ָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "********������·�ָ�������⣬���ܺ�[%d]****", (int)boxNumber);
		p->NotifyMsg(tBuf);
		SZL_INFO("������·��������ָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}
void TcpServer::AddGoodsBoxOutReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "�յ�plc���͵Ļ��䲹����������ϱ�");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ĳ�����������ϱ���Ϣ");
	ServerAddGoodsBoxout* pReport = (ServerAddGoodsBoxout*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ�������������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
	}
	delete rep;
	rep = nullptr;
}
void TcpServer::AddGoodsBoxInReport(SOCKET sock, char* str)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	sprintf_s(tBuf, "�յ�plc���͵Ļ��䲹���������ϱ�");
	p->NotifyMsg(tBuf);
	SZL_INFO("�յ�plc���͵Ĳ����������ϱ���Ϣ");
	ServerAddGoodsBoxin* pReport = (ServerAddGoodsBoxin*)str;

	SendDataHeader* rep = new SendDataHeader;
	rep->_msgType = CLIENT_MSG_TYPE_CMD::REPORT_REP;
	memcpy(rep->_key, pReport->_key, sizeof(pReport->_key));
	rep->_devType = pReport->_devType;
	rep->_devId = pReport->_devId;

	if (SOCKET_ERROR == SendData(sock, rep))
	{
		SZL_ERROR("�ظ������������ϱ�ʧ�ܣ�������{0}", WSAGetLastError());
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
	//�ֽ�ת��Ϊ������
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

	sprintf_s(buf,"����λ��: X:%.2f,Y:%.2f,Z:%.2f", xpos.value, ypos.value, zpos.value);
	p->BoxPosUpdate(buf);
}
void TcpServer::RobotArmPutGoodsEnd(bool bSucess)
{
	CPlcFaceDlg* p = (CPlcFaceDlg*)m_PlcDlg;
	char tBuf[1024] = { 0 };
	if (!bSucess)
	{
		SZL_WARN("��е�۷Ż�ʧ��.##########��");
		sprintf_s(tBuf, "**********��е�۷Ż�ʧ��********��");
	}
	else
	{
		sprintf_s(tBuf, "**********��е�۷Ż��ɹ�********��");
		SZL_INFO("��е�۷Ż���ɡ�");
	}
	p->NotifyMsg(tBuf);
	m_MaxTakeNum++;
	if (m_positionInfoList.size() <= 0)
		return;
	SZL_INFO("��؛؛�䔵��:{0}", m_positionInfoList.size());
	auto itB = m_positionInfoList.begin();
	auto itBcell = itB->vecCellInfo.begin();
	SZL_INFO("���Ӻţ�{0},��ӦҪ��������{1}", itBcell->cellNum, itBcell->outputGoodsNum);
	//�ɹ�����������һ
	itBcell->outputGoodsNum--;
	itBcell->cellStock--;
	SZL_INFO("���Ӻţ�{0},��ӦҪ��������{1}", itBcell->cellNum,itBcell->outputGoodsNum);
	//��������Ʒ��������5���򿪽�����
	if (m_MaxTakeNum >= 5)
	{
		//�򿪽�����
		memset(tBuf, 0X00, sizeof(tBuf));
		sprintf_s(tBuf, "************��������Ʒ��������5���򿪽�����***********");
		p->NotifyMsg(tBuf);
		SZL_INFO("************��������Ʒ��������5���򿪽�����***********");
		openSafeDoor();
		m_MaxTakeNum = 0;
		m_isOpenDoor = true;
	}
	if (itBcell->outputGoodsNum > 0)
	{
		//�·����Ӻţ�λ�õ���Ϣ����е�ۣ�ȡ��
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
				//�·����Ӻţ�λ�õ���Ϣ����е�ۣ�ȡ��
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
		SZL_INFO("��؛؛�䔵��:{0}", m_positionInfoList.size());
		m_positionInfoList.erase(itB);
		SZL_INFO("��؛؛�䔵��:{0}", m_positionInfoList.size());
		//�����ƽ�ȥ
		goodsBoxIn(m_BInNum);
		m_IsBoxEx = false;
	}
	if (m_positionInfoList.size() == 0)
	{
		//��������ȡ��
		//�򿪽�����
		memset(tBuf, 0X00, sizeof(tBuf));
		sprintf_s(tBuf,"***************��������ȡ�꣬�򿪽�����*************");
		p->NotifyMsg(tBuf);
		SZL_INFO("***************��������ȡ�꣬�򿪽�����*************");
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
	SZL_INFO("�����ļ���ȡ���Ķ˿ں�1:{0},�˿ں�2:{1},ip��ַ:{2},��������ʱ����:{3}s", _port,_port2,_ip, _intervalBeatConnect);
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
			SZL_ERROR("<socket=%d>,�O���B������æµ", (int)it->sockfd());
			continue;
		}*/
		pSend->_key[0] = ToolHelp::GetRandNumber();
		pSend->_key[1] = ToolHelp::GetRandNumberS();
		//pSend->_boxNumber = boxNumber;
		if (SOCKET_ERROR == SendData(it->sockfd(), pSend))
		{
			SZL_ERROR("������λָ���·�ʧ�ܣ�������{0}", WSAGetLastError());
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
		sprintf_s(tBuf, "******** ������·�ָ�������λ ****");
		p->NotifyMsg(tBuf);
		SZL_INFO("������·�������λָ��_sendType={0},plcSock={1}", (int)pSend->_sendType, (int)it->sockfd());
	}
}