/*
* @Abrief Eposn机械臂
* @note Eposn机械臂通过远程以太网连接，拥有操作机械臂
* @version 1.0
* @author wuxiangchao 2023-2-7
*/

#ifndef _EPSONROBOT_DRIVER_H
#define _EPSONROBOT_DRIVER_H

#include <QMap>
#include "BgiSocketCommDriver.h"
#include "BgiThread.h"
#include "RobotParaType.h"
#include "BusinessExternDef.h"

class BgiTcpSocket;
class SchedulerDriver;

class BGIBUSINE_CLASS_EXPORT EpsonRobotDriver : public BgiSocketCommDriver, public BgiThread
{
	Q_OBJECT
public:
	EpsonRobotDriver(BgiTcpSocket* socket);
	virtual ~EpsonRobotDriver();

	void StartRead();
	void EndRead();
	void SendCommand(QString strCommand,bool bRecord = true);

	/* --------------------------机械臂指令集 开始-------------------------------------*/
	void AutoGetPara();
	void GetCurPos();
	void Login();
	bool Logout();
	bool GetCurRobot(int &iMotorNo);
	bool SetMotorsOn();
	bool SetMotorsOff();
	bool Reset();
	bool Pause();
	bool Stop();
	bool Continue();
	bool Abort();
	bool SetPower(bool bHigh);
	bool SetPTP_Speed(int iSpeed);
	bool GetPTP_Speed(int &iSpeed);
	bool SetPTP_Accel(int iAccel);
	bool GetPTP_Accel(int &iAccel);
	bool SetCP_Speeds(int iSpeed);
	bool GetCP_Speeds(int &iSpeed);
	bool SetCP_Accels(int iAccel);
	bool GetCP_Accels(int &iAccel);
	bool SetR_Speedr(int iSpeed);
	bool GetR_Speedr(int& iSpeed);
	bool SetR_Accelr(int iAccel);
	bool GetR_Accelr(int& iAccel);
	bool GoBasePoint();
	bool GoHome();
	//注重轨迹控制时用move，注重到达目标点
	bool Execute_Go(QString strPosVar);
	bool Execute_Pass(QString strPosVar);
	bool Execute_Move(QString strPosVar);
	//输出IO bit
	bool On_OutputIO(int iIObit);
	bool On_OutputIO(QString strIOName);
	bool Off_OutputIO(int iIObit);
	bool Off_OutputIO(QString strIOName);
	//读取指定输出位的状态
	bool Oport_OutputIO(int iIObit, int& iStatus);
	bool Oport_OutputIO(QString strIOName, int& iStatus);
	bool Oport_OutputIO_Auto(int iIObit, int& iStatus);
	bool Oport_OutputIO_Auto(QString strIOName, int& iStatus);
	//读取输入IO bit
	bool Sw_InputIO(int iIObit,int &iStatus);
	bool Sw_InputIO(QString strIOName, int &iStatus);
	bool Sw_InputIO_Auto(int iIObit, int& iStatus);
	bool Sw_InputIO_Auto(QString strIOName, int& iStatus);
	//@type 0:X 1:Y 2:Z 3:U 4:V 5:W
	bool Execute_ConsecutiveSport(RobotParaType::RobotSportMode mode);
	/* --------------------------机械臂指令集 结束-------------------------------------*/
	// 判断夹手打开是否成功
	bool IsOpenGripperSuccess();
	// 打开夹爪
	bool OpenGripper();
	// 判断是否夹取芯片成功
	bool IsGripChipSuccess();
	// 关闭夹爪
	bool CloseGripper();
	// 初始化机械臂
	bool MotorInit();
	// 设置Move低速
	bool MotorSetLowSpeed();
	// 设置Go高速
	bool MotorSetHighSpeed();
	// 风刀口吹风
	bool BlowChip(bool bOpen);
	// 操作蜂鸣器
	bool OperateBuzzer(bool bOpen);
	// 操作信号灯
	bool OperateWaringLights(RobotParaType::RobotLightColor color);
	// 判断中转槽是否有芯片
	bool HaveChipInTransferTank(int iSlotID);
	// 判断是否到达点位
	bool IsMoveToPositionSuccess(QString strPosSign);
	// 判断机械臂是否满足移动条件
	bool IsMotorReadyMove();
	// 判断机械臂是否意外停止
	bool IsMoveStopUnexpected();
	// 判断在安全工作空间
	bool IsInSafetyWorkSpace();
	// 判断是否处于高功率
	bool IsMotorPowerHigh();
	// 断开网络连接
	void CloseRobotSocket();
	// 返回机械臂网络连接状态
	bool GetConnectStatus();
	void TestShowMsgBox();
signals:
	void SignalMasterCloseRobotConnect();

public slots:
	void SlotConnectStatus(bool bSuccess);

private:
	void ApplicationSleep(int mSec = 100);
	void InitCalibPointsInfo();
	QString CheckStringPoint(const RobotParaType::RobotPoint);
	int CmpPoints(const void* arg1, const void* arg2);
	//	记录Here的点位名称或者最接近Here的点位名称
	void RecordNearPointNameOfHere();
	void ExplainBuff(QByteArray); //串口传输出来的状态信息
	virtual void Run(void);
	int NeedBreakExplainBuff(QString strBuf, QString indexOfVar);
	int ReturnPaseValueToInt(QString strBUffer, QString KeyWords);
	void TryPraseQueryOutputValueInThread(QByteArray buf);
	bool TryPraseQueryOutputValueByKeyWords(QString keyWord, QVariant& Value);
	bool TryPraseQueryOutputValueByKeyWords(QString keyWord);
	bool ConductCommandUnit(QString strCommand,QString keyWord, QVariant& Value);
	bool OperateIODeviceUnit(int iIO, bool bOpen);
	bool GetRobotSpeedPara(QString strKey, int iMax,int &iValue);
private:
	BgiTcpSocket* m_pSocket;
	bool m_bEnd;
	bool m_bLogin;
	RobotParaType::RobotStatus m_strRobotStatus;
	RobotParaType::RobotPoint m_strPoint;
	RobotParaType::RobotPara m_RobotParaDesc;

	QByteArray m_curBuf;
	bool m_bGetPara;
	bool m_bAutoExplainBuff;
	bool m_bSetPara;
	//执行动作
	bool m_bAtAction; //避免失败后动作执行两次的异常
	bool m_bRobotConnect;
	bool m_bEmergencyStop;
	//安全空间
	bool m_bAtSafetyWorkSpace;
	int m_iErrCount;
	int m_iErrCount_21;
	//已校准点Map，用于保存已经较好的点的具体位置
	QMap<QString, RobotParaType::RobotPoint> m_mapCalibPoints;
	bool m_bInitCalibPointsInfoSuc;

	// 调度driver
	SchedulerDriver* m_pSchedulerDriver;

	//Python 执行的动作
	bool m_bPythonMove;
	//Python 下意外停止（遇到碰撞检测点）
	bool m_bStopUnexpected;
	//速度切换标记
	bool m_bChangeToGo, m_bChangeToMove;
	// ExplainBuff finish
	bool m_bExplainBuffFinish;
	// 机械臂是否停止中转槽
	bool m_bAtTferWait;
	// 是否正在操作警告灯
	bool m_OperateLightsing;
};

#endif