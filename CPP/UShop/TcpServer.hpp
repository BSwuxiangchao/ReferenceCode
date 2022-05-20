//copyright 2021 dragon@oct

//Tcp服务端类
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


//设备信息
struct DevInfo
{
	char _devType=0X02;					  //设备类型
	char _devId=0X64;					  //设备id
};
enum SockIdType
{
	PLC_CLIENT=1,                //plc客户端
	POS_CLIENT,                   //位置
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
	SockIdType _sockId= SockIdType::PLC_END;                             //连接标识，1:plc
	//第二缓冲区，消息缓冲区
	char _szMsgBuff[RECV_BUFF_SIZE * 2] = {};//双缓冲
	int _lastPos;
	time_t _PreHeartTime;                    //前一次心跳时间
public:
	PlcDevState _devState;                   //设备状态
};

class TcpServer:public MyThread
{
public:
	TcpServer();
	virtual ~TcpServer();
	void Close();
	//初始化socket
	SOCKET IntiSocket();
	//绑定端口号
	int Bind();
	//监听网络端口
	int Listen(int n);
	//接收客户端连接
	SOCKET Accept();
	SOCKET Accept2();
	bool OnRun();
	bool isRun();
	int RecvData(ClientSocket* pClient);
	//处理客户端发送的信息
	virtual void OnNetMsg(SOCKET cSock,char* str);
	int SendData(SOCKET cSock, DataHeader* header);
	void SendDataToAll(DataHeader* header);
	//检测重发线程函数
	void Routine()override final;
public:
	//获取设备状态
	PlcDevState GetDevState(SOCKET sock);
	//获取设备状态
	PlcDevState GetDevState();
	//设置主窗体
	void SetPlcDlg(void * p) { m_PlcDlg =p; }
	//设置补货出库的列表
	void SetCellInfoList(std::vector<CompleteBox> ve) { m_productCellInfoList = ve; }
	////设置出货位置信息
	void SetOutBoxInfo(std::vector<CompleteBox> ve) {m_positionInfoList=ve; }
	//继续上货货箱出库
	bool ContinueOutBox();
	//继续出货货箱出库
	bool ContOutBox();
	//获取上货货架号
	inline int GetCurBoxNum() { return m_curBoxNum; }  
	//获取出货货架号
	inline int GetBinNum() { return m_BInNum; }		
	inline void SetCurBoxNum(int shelfId) { m_curBoxNum=shelfId; }
	inline void SetBinNum(int shelfId) {  m_BInNum=shelfId; }		
	//读取配置文件
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

	bool RepeatSendCmd(SOCKET csock,DataHeader* str, std::pair<ServerKey,TimeCmd>& pair);         //重发指令
	bool disConnectClientTimeOut(ClientSocket* pClient);      //超时断连
	//服务端回复plc消息
	void heartBeat(SOCKET cSock, char* str);          //心跳
	void Login(SOCKET cSock, char* str);              //登录
	void Report(SOCKET cSock, char* str);             //上报

	//服务端回复plc上报消息
	void stateReport(SOCKET sock, char* str);           //设备状态上报
	void boxOutReport(SOCKET sock, char* str);			//货箱出库完成上报
	void boxInReport(SOCKET sock, char* str);			//货箱入库完成上报
	/***********************流程調整，這些接口废弃*************************/
	void robarmTakeReport(SOCKET sock, char* str);      //机械臂请求取货上报
	void robarmTakeEndReport(SOCKET sock, char* str);   //机械臂取货完成上报
	void robarmPutEndReport(SOCKET sock, char* str);    //机械臂放货完成上报
	/***********************************************************************/
	void customerTakeEndReport(SOCKET sock, char* str); //顾客取商品完成上报
	void boxTransforOutReport(SOCKET sock, char* str);  //货箱转运出库完成上报
	void boxTransforInReport(SOCKET sock, char* str);   //货箱转运入库完成上报
	void AddGoodsBoxOutReport(SOCKET sock, char* str);  //货箱补货出库完成上报
	void AddGoodsBoxInReport(SOCKET sock, char* str);   //货箱补货入库完成上报
	void XYZPosReport(SOCKET sock, char* str);          //xyz坐标上传

	//客户端回复控制指令
	void ClientReponse(SOCKET sock, char* str);
	//解析客户端回复指令
	void ParseReponse(DataHeader* head);
	//下发格子号等信息
	void SendCellInfoToRobot(std::vector<CompleteBox>::iterator itB, std::vector<CellInfoPLC>::iterator itBcell);
	void WarningHandle(const char* buf);
public:
	//服务端下发控制指令
	void goodsBoxOut(char boxNumber/*货架编号*/,const char* taskNumber=nullptr/*任务号*/);        //货箱出库
	bool goodsBoxIn(char boxNumber/*货架编号*/,const char* taskNumber =nullptr/*任务号*/);       //货箱入库

	/***********************流程調整，這些接口废弃*************************/
	void robarmTakeGoodsContinue();																 //机械臂继续取货
	/***********************************************************************/

	void openSafeDoor();																//打开交付安全门
	void goodsBoxTransforOut(char boxNumber/*货架编号*/);								//货箱转运出库
	void goodsBoxTransforIn(char boxNumber/*货架编号*/);								//货箱转运入库
	void resetPos();																	//复位
	void startPlc();																	//启动
	void stopPlc();																		//停止
	void AddGoodsBoxOut(char boxNumber/*货架编号*/);									//补货出库
	void AddGoodsBoxIn(char boxNumber/*货架编号*/);										//补货入库
	void PlcWarnReset();                                                                //报警复位
	//机械臂调用接口
	void RobotArmPutGoodsEnd(bool bSucess);                  //机械臂放货完成
	void RobotArmTakeGoodsEnd();                             //机械臂取货完成
public:
	PlcWarning m_plcWarning;
	int _nCount = 0;
	char _szRecv[RECV_BUFF_SIZE] = {0};
private:
	SOCKET _sock;                                 //服务端sock
	SOCKET _sock2;                                //服务端sock2
	int _port;                                    //端口号
	int _port2;                                   //端口号2
	std::string _ip;                              //ip
	int _intervalBeatConnect=30;                  //心跳超时时间
	std::vector<ClientSocket*> _clients;          //连接到的客户端sock
	ServerCmdMap _serverSendCmd;                  //服务端下发的控制指令记录
	std::thread* m_th = nullptr;                  //重发线程
	DevInfo _devInfo;                             //设备信息

	std::vector<CompleteBox> m_productCellInfoList;   //需要推出的货箱列表
	std::vector<CompleteBox> m_positionInfoList;      //出货位置
	bool m_isStopRouine = false;       //停止重发线程
	void* m_PlcDlg = nullptr;
	int m_curBoxNum = -1;              //上货当前货箱号
	int m_BInNum = -1;                 //出货当前货架号
	int m_MaxTakeNum = 0;              //最大取货数，到达此值时打开交付口
	bool m_isOpenDoor=false;           //交付口是否打开
	bool m_IsBoxEx = false;            //是否有货箱可以取货
	bool m_IsBoxIn = false;            //货箱入库成功
	bool m_AddBoxIn = false;           //补货货箱入库是否成功
	bool m_IsOutGoodsEnd = true;       //出货成功
	bool m_IsLoadGoodsEnd = true;       //入库成功
	bool m_IsAIstoreMode = false;	   //语音上货
	bool m_bSecondPushBox = false;		//是否自动推出货箱
	bool m_IsStartUp = false;           //是否新启动程序
	bool m_isBtnBox = false;            //货箱是否手动出库入库
	bool m_armMachineOptState = true;	//机器臂抓取货物的操作状态
	UshopService m_pickUpGoodsService;
};