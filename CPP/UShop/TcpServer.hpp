//copyright 2021 dragon@oct

//Tcp�������
/* $Header:PlcServer/TcpServer.hpp     5/24/2021 16:39                   */
/**************************************************************************
***                                                                     ***
*                                                                         *
*                    Project Name: PlcServer                              *
*                                                                         *
*                       File Name: TcpServer.hpp                          *
*                                                                         *
*                      Start Date: 5/24/2021                              *
*                                                                         *
*                       Last Date: 5/28/2021                              *
*-------------------------------------------------------------------------*
* Functions:                                                              *
*-------------------------------------------------------------------------*
**************************************************************************/



#pragma once
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"stdafx.h"

#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")

#include"MessageHeader.hpp"
#include"MyThread.h"
#include "UshopService.h"
#include"Profile.h"
#include"PlcWarning.h"
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 4096
#endif // !RECV_BUFF_SIZE

using  TimeCmd = std::pair<DataHeader*, SendRecvTime>;
using ServerCmdMap = std::map<ServerKey, TimeCmd>;


//�豸��Ϣ
struct DevInfo
{
	char _devType=0X02;					  //�豸����
	char _devId=0X64;					  //�豸id
};
enum SockIdType
{
	PLC_CLIENT=1,                //plc�ͻ���
	POS_CLIENT,                   //λ��
	PLC_END
};
class ClientSocket
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET);
	SOCKET sockfd()
	{
		return _sockfd;
	}
	inline char* msgBuf()
	{
		return _szMsgBuff;
	}
	inline int getLastPos()
	{
		return _lastPos;
	}
	inline void setLastPos(int pos)
	{
		_lastPos = pos;
	}
	inline time_t getPreHeartTime()
	{
		return _PreHeartTime;
	}
	inline void setHeartTime(time_t nowt)
	{
		_PreHeartTime = nowt;
	}
	inline void SetSockId(SockIdType id)
	{
		_sockId = id;
	}
	inline SockIdType GetSockId()
	{
		return _sockId;
	}
private:
	SOCKET _sockfd;
	SockIdType _sockId= SockIdType::PLC_END;                             //���ӱ�ʶ��1:plc
	//�ڶ�����������Ϣ������
	char _szMsgBuff[RECV_BUFF_SIZE * 2] = {};//˫����
	int _lastPos;
	time_t _PreHeartTime;                    //ǰһ������ʱ��
public:
	PlcDevState _devState;                   //�豸״̬
};

class TcpServer:public MyThread
{
public:
	TcpServer();
	virtual ~TcpServer();
	void Close();
	//��ʼ��socket
	SOCKET IntiSocket();
	//�󶨶˿ں�
	int Bind();
	//��������˿�
	int Listen(int n);
	//���տͻ�������
	SOCKET Accept();
	SOCKET Accept2();
	bool OnRun();
	bool isRun();
	int RecvData(ClientSocket* pClient);
	//����ͻ��˷��͵���Ϣ
	virtual void OnNetMsg(SOCKET cSock,char* str);
	int SendData(SOCKET cSock, DataHeader* header);
	void SendDataToAll(DataHeader* header);
	//����ط��̺߳���
	void Routine()override final;
public:
	//��ȡ�豸״̬
	PlcDevState GetDevState(SOCKET sock);
	//��ȡ�豸״̬
	PlcDevState GetDevState();
	//����������
	void SetPlcDlg(void * p) { m_PlcDlg =p; }
	//���ò���������б�
	void SetCellInfoList(std::vector<CompleteBox> ve) { m_productCellInfoList = ve; }
	////���ó���λ����Ϣ
	void SetOutBoxInfo(std::vector<CompleteBox> ve) {m_positionInfoList=ve; }
	//�����ϻ��������
	bool ContinueOutBox();
	//���������������
	bool ContOutBox();
	//��ȡ�ϻ����ܺ�
	inline int GetCurBoxNum() { return m_curBoxNum; }  
	//��ȡ�������ܺ�
	inline int GetBinNum() { return m_BInNum; }		
	inline void SetCurBoxNum(int shelfId) { m_curBoxNum=shelfId; }
	inline void SetBinNum(int shelfId) {  m_BInNum=shelfId; }		
	//��ȡ�����ļ�
	BOOL Open(LPCTSTR profile, LPCTSTR section);
	inline void SetOutGoodsState(bool state) 
	{
		m_IsOutGoodsEnd=state;
	}
	inline bool GetOutGoodsState() {
		return m_IsOutGoodsEnd;
	}
	inline void SetLoadGoodsState(bool state)
	{
		m_IsLoadGoodsEnd = state;
	}
	inline bool GetLoadGoodsState()
	{
		return m_IsLoadGoodsEnd;
	}
	inline void SetAIstoreMode(bool mode)
	{
		m_IsAIstoreMode = mode;
	}
	inline void SetAutoPushBoxState(bool bAuto)
	{
		m_bSecondPushBox = bAuto;
	}
	inline void SetIsStartUp(bool bl)
	{
		m_IsStartUp = bl;
	}
	inline void SetIsBtnBox(bool bl)
	{
		m_isBtnBox = bl;
	}
	inline bool GetArmMachineOptState()
	{
		return m_armMachineOptState;
	}
	inline void SetArmMachineOptState(bool state)
	{
		m_armMachineOptState = state;
	}
private:  

	bool RepeatSendCmd(SOCKET csock,DataHeader* str, std::pair<ServerKey,TimeCmd>& pair);         //�ط�ָ��
	bool disConnectClientTimeOut(ClientSocket* pClient);      //��ʱ����
	//����˻ظ�plc��Ϣ
	void heartBeat(SOCKET cSock, char* str);          //����
	void Login(SOCKET cSock, char* str);              //��¼
	void Report(SOCKET cSock, char* str);             //�ϱ�

	//����˻ظ�plc�ϱ���Ϣ
	void stateReport(SOCKET sock, char* str);           //�豸״̬�ϱ�
	void boxOutReport(SOCKET sock, char* str);			//�����������ϱ�
	void boxInReport(SOCKET sock, char* str);			//�����������ϱ�
	/***********************�����{�����@Щ�ӿڷ���*************************/
	void robarmTakeReport(SOCKET sock, char* str);      //��е������ȡ���ϱ�
	void robarmTakeEndReport(SOCKET sock, char* str);   //��е��ȡ������ϱ�
	void robarmPutEndReport(SOCKET sock, char* str);    //��е�۷Ż�����ϱ�
	/***********************************************************************/
	void customerTakeEndReport(SOCKET sock, char* str); //�˿�ȡ��Ʒ����ϱ�
	void boxTransforOutReport(SOCKET sock, char* str);  //����ת�˳�������ϱ�
	void boxTransforInReport(SOCKET sock, char* str);   //����ת���������ϱ�
	void AddGoodsBoxOutReport(SOCKET sock, char* str);  //���䲹����������ϱ�
	void AddGoodsBoxInReport(SOCKET sock, char* str);   //���䲹���������ϱ�
	void XYZPosReport(SOCKET sock, char* str);          //xyz�����ϴ�

	//�ͻ��˻ظ�����ָ��
	void ClientReponse(SOCKET sock, char* str);
	//�����ͻ��˻ظ�ָ��
	void ParseReponse(DataHeader* head);
	//�·����Ӻŵ���Ϣ
	void SendCellInfoToRobot(std::vector<CompleteBox>::iterator itB, std::vector<CellInfoPLC>::iterator itBcell);
	void WarningHandle(const char* buf);
public:
	//������·�����ָ��
	void goodsBoxOut(char boxNumber/*���ܱ��*/,const char* taskNumber=nullptr/*�����*/);        //�������
	bool goodsBoxIn(char boxNumber/*���ܱ��*/,const char* taskNumber =nullptr/*�����*/);       //�������

	/***********************�����{�����@Щ�ӿڷ���*************************/
	void robarmTakeGoodsContinue();																 //��е�ۼ���ȡ��
	/***********************************************************************/

	void openSafeDoor();																//�򿪽�����ȫ��
	void goodsBoxTransforOut(char boxNumber/*���ܱ��*/);								//����ת�˳���
	void goodsBoxTransforIn(char boxNumber/*���ܱ��*/);								//����ת�����
	void resetPos();																	//��λ
	void startPlc();																	//����
	void stopPlc();																		//ֹͣ
	void AddGoodsBoxOut(char boxNumber/*���ܱ��*/);									//��������
	void AddGoodsBoxIn(char boxNumber/*���ܱ��*/);										//�������
	void PlcWarnReset();                                                                //������λ
	//��е�۵��ýӿ�
	void RobotArmPutGoodsEnd(bool bSucess);                  //��е�۷Ż����
	void RobotArmTakeGoodsEnd();                             //��е��ȡ�����
public:
	PlcWarning m_plcWarning;
	int _nCount = 0;
	char _szRecv[RECV_BUFF_SIZE] = {0};
private:
	SOCKET _sock;                                 //�����sock
	SOCKET _sock2;                                //�����sock2
	int _port;                                    //�˿ں�
	int _port2;                                   //�˿ں�2
	std::string _ip;                              //ip
	int _intervalBeatConnect=30;                  //������ʱʱ��
	std::vector<ClientSocket*> _clients;          //���ӵ��Ŀͻ���sock
	ServerCmdMap _serverSendCmd;                  //������·��Ŀ���ָ���¼
	std::thread* m_th = nullptr;                  //�ط��߳�
	DevInfo _devInfo;                             //�豸��Ϣ

	std::vector<CompleteBox> m_productCellInfoList;   //��Ҫ�Ƴ��Ļ����б�
	std::vector<CompleteBox> m_positionInfoList;      //����λ��
	bool m_isStopRouine = false;       //ֹͣ�ط��߳�
	void* m_PlcDlg = nullptr;
	int m_curBoxNum = -1;              //�ϻ���ǰ�����
	int m_BInNum = -1;                 //������ǰ���ܺ�
	int m_MaxTakeNum = 0;              //���ȡ�����������ֵʱ�򿪽�����
	bool m_isOpenDoor=false;           //�������Ƿ��
	bool m_IsBoxEx = false;            //�Ƿ��л������ȡ��
	bool m_IsBoxIn = false;            //�������ɹ�
	bool m_AddBoxIn = false;           //������������Ƿ�ɹ�
	bool m_IsOutGoodsEnd = true;       //�����ɹ�
	bool m_IsLoadGoodsEnd = true;       //���ɹ�
	bool m_IsAIstoreMode = false;	   //�����ϻ�
	bool m_bSecondPushBox = false;		//�Ƿ��Զ��Ƴ�����
	bool m_IsStartUp = false;           //�Ƿ�����������
	bool m_isBtnBox = false;            //�����Ƿ��ֶ��������
	bool m_armMachineOptState = true;	//������ץȡ����Ĳ���״̬
	UshopService m_pickUpGoodsService;
};