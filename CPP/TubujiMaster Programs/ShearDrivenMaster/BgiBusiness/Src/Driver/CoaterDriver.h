#pragma once

#include <QObject>
#include "BgiSocketCommDriver.h"
#include <QTimer>
#include "BusinessExternDef.h"
#include "CoaterParaType.h"
#include <QMap>

enum PLCDataType
{
	ALARM_TYPE = 0,
	STATUS_TYPE,
	COMMAND_TYPE,
	OTHER_TYPE
};

class BGIBUSINE_CLASS_EXPORT CoaterDriver  : public BgiSocketCommDriver
{
	Q_OBJECT

public:
	CoaterDriver(BgiTcpSocket* socket);
	~CoaterDriver();

	void GetConnStatus();

	//是否可以上料
	bool IsLoadState();
	//是否可以下料
	bool IsUnloadState();
	//开关真空
	bool SwitchVacuum(bool bOpen);
	//获取真空值
	float GetVacuumValue();
	//上料完成
	bool SetLoadFinish();
	//下料完成
	bool SetUnloadFinish();

	//启动
	bool Startup();
	//暂停
	bool Pause(bool bPause);
	//停止
	bool Stop();
	//急停
	bool Halt();
	//复位
	bool Reset();
	//报警重置
	bool AlarmReset();
	//读额外的命令区
	void ReadCmdData(bool bRead);

	//更新机械臂进入状态
	void UpdateRobotSafetyStatus(bool bSafety);

signals:
	void SignalCoaterConnectStatus(bool bSuccess);
	void SignalNewData(PLCDataType, const QVector<int>&);
	void SignalDeviceStatus(int, int);
	void SignalVacuumValue(int, float);
	void SignalPumpStatus(int, int);
	void SignalPumpValue(int, int);
	void SignalPumpSize(int, int);
	void SignalNewAlarm(QString);
	void SignalNewStep(QString);
	void SignalAllAlarm(const QMap<int, QString>&);
	void SignalCoaterSafePos(bool);
	void SignalCoatingResult(int);

public slots:
	//涂布机的连接状态
	void SlotConnectStatus(bool bSuccess);
	//查询PLC的数据
	void QueryPLCData();

private:
	//void ConnectSocketSlot();

	//初始化PLC地址对应的数据信息
	void InitPLCAddr();
	void ResetPLCValue();

	//网口来的数据
	void ExplainBuff(QByteArray buf);

	//清空存储的数据
	void ClearBuff();
	//获取一条完整的PLC数据
	QString GetPLCMsg();

	//更新缓存的数据
	QMap<int, int> UpdateData(const QStringList& data, QVector<int>& saveData);
	void UpdateStatusData(const QStringList& data);
	void PLCStatusChenged(QString key, int addr, int value);

	void SendNewAlarm(const QMap<int, int>& values);
	void SendAllAlarm(const QVector<int>& alarmData);
	QString GetAlarmString(int addr, int value);
	QString GetResetStepString(int addr, int value);
	QString GetFlowStepString(int addr, int value);

	float IntToFloat(int value);

	bool ResetLoadStatus();
	bool ResetUnloadStatus();
	//设置机械手处于安全位置状态
	bool SetSafetyStatus(bool bSafety);

	//下发触发式命令
	bool SendTriggerCommand(QString cmdKey);
	//向PLC下发写入命令
	bool SendWriteCommand(QString cmd);
	//向PLC发送命令
	bool SendCommand(QString cmd, bool bLog = false);
	//等待PLC应答
	bool WaitReplay();

private:
	BgiTcpSocket* m_pSocket;
	QTimer* m_pTimer;
	QThread* m_pThread;
	BgiLock m_SendLock;

	bool m_Connected;
	bool m_bSafePos;
	bool m_CanStartup;  //是否可启动涂布机

	//收到的网口数据
	QByteArray m_RecvBuf;

	//是否在等待PLC的回复
	bool m_WaitReply;

	QString m_QueryAlramStr;
	QString m_QueryStatusStr;
	QString m_QueryCommandStr;
	bool m_bReadCommand;

	PLCDataType m_DataType;  //当前接收数据的类型
	QVector<int> m_AlarmArray;  //涂布机PLC的报警列表
	QVector<int> m_StatusArray;  //涂布机PLC的状态列表
	QVector<int> m_CmdArray;  //写入涂布机PLC的命令列表

	QMap<QString, CoaterParaType::PLCStatusData> m_PLCStatusDatas;
	QMap<QString, QString> m_PLCCmdDatas;
};
