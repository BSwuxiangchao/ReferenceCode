#include "CoaterDriver.h"
#include "BgiTcpSocket.h"
#include "DeviceDetailCfg.h"
#include "BgiLogger.h"
#include "DeviceErrorDescription.h"
#include "CoaterStepDescription.h"

CoaterDriver::CoaterDriver(BgiTcpSocket* socket)
	: BgiSocketCommDriver()
	, m_pSocket(socket)
	, m_Connected(false)
	, m_WaitReply(false)
	, m_bReadCommand(false)
	, m_bSafePos(false)
	, m_CanStartup(false)
{
	SetTcpSocket(m_pSocket);

	ConnectSocketSlot();

	m_AlarmArray.resize(100);
	m_StatusArray.resize(100);
	m_CmdArray.resize(100);

	DeviceDetailCfg::Instance()->GetCfgValue("CoaterCfg/QueryAlram", m_QueryAlramStr);
	DeviceDetailCfg::Instance()->GetCfgValue("CoaterCfg/QueryStatus", m_QueryStatusStr);
	DeviceDetailCfg::Instance()->GetCfgValue("CoaterCfg/QueryCommand", m_QueryCommandStr);

	m_pThread = new QThread();
	m_pThread->start(QThread::NormalPriority);
	this->moveToThread(m_pThread);

	InitPLCAddr();

	m_pTimer = new QTimer(this);
	QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(QueryPLCData()));
	m_pTimer->start(1000);
}

CoaterDriver::~CoaterDriver()
{
	DisConnectSocketSlot();

	m_pTimer->stop();
	m_pTimer->deleteLater();

	m_pThread->quit();
	m_pThread->wait();
	m_pThread->deleteLater();
}

//void CoaterDriver::ConnectSocketSlot()
//{
//	QObject::connect(m_pSocket, SIGNAL(SendBuf(QByteArray)), this, SLOT(GetSocketBuff(QByteArray)), Qt::DirectConnection);
//}

void CoaterDriver::InitPLCAddr()
{
	//状态地址
	m_PLCStatusDatas.clear();
	m_PLCStatusDatas.insert("DeviceStatus", CoaterParaType::PLCStatusData(7200, 1));
	m_PLCStatusDatas.insert("StartReady", CoaterParaType::PLCStatusData(7201, 1));//可启动条件
	m_PLCStatusDatas.insert("DeviceMode", CoaterParaType::PLCStatusData(7202, 1));
	m_PLCStatusDatas.insert("PETStatus", CoaterParaType::PLCStatusData(7203, 1));//膜状态   1:需上膜     2:需下膜

	m_PLCStatusDatas.insert("FlowStep", CoaterParaType::PLCStatusData(7205, 1));//流程步序号

	m_PLCStatusDatas.insert("ChipVacuum", CoaterParaType::PLCStatusData(7206, 2));
	m_PLCStatusDatas.insert("PETVacuum", CoaterParaType::PLCStatusData(7208, 2));

	m_PLCStatusDatas.insert("ResetReady", CoaterParaType::PLCStatusData(7210, 1));//可复位条件

	m_PLCStatusDatas.insert("LoadStatus", CoaterParaType::PLCStatusData(7211, 1));
	m_PLCStatusDatas.insert("UnloadStatus", CoaterParaType::PLCStatusData(7212, 1));

	m_PLCStatusDatas.insert("ResetStep", CoaterParaType::PLCStatusData(7213, 1));//复位步序号

	m_PLCStatusDatas.insert("PumpStatus1", CoaterParaType::PLCStatusData(7214, 1));
	m_PLCStatusDatas.insert("PumpStatus2", CoaterParaType::PLCStatusData(7215, 1));
	m_PLCStatusDatas.insert("PumpStatus3", CoaterParaType::PLCStatusData(7216, 1));
	m_PLCStatusDatas.insert("PumpStatus4", CoaterParaType::PLCStatusData(7217, 1));
	m_PLCStatusDatas.insert("PumpStatus5", CoaterParaType::PLCStatusData(7218, 1));
	m_PLCStatusDatas.insert("PumpStatus6", CoaterParaType::PLCStatusData(7219, 1));
	m_PLCStatusDatas.insert("PumpStatus7", CoaterParaType::PLCStatusData(7220, 1));
	m_PLCStatusDatas.insert("PumpStatus8", CoaterParaType::PLCStatusData(7221, 1));
	m_PLCStatusDatas.insert("PumpStatus9", CoaterParaType::PLCStatusData(7222, 1));
	m_PLCStatusDatas.insert("PumpStatus10", CoaterParaType::PLCStatusData(7223, 1));

	m_PLCStatusDatas.insert("PumpValue1", CoaterParaType::PLCStatusData(7225, 1));
	m_PLCStatusDatas.insert("PumpValue2", CoaterParaType::PLCStatusData(7226, 1));
	m_PLCStatusDatas.insert("PumpValue3", CoaterParaType::PLCStatusData(7227, 1));
	m_PLCStatusDatas.insert("PumpValue4", CoaterParaType::PLCStatusData(7228, 1));
	m_PLCStatusDatas.insert("PumpValue5", CoaterParaType::PLCStatusData(7229, 1));
	m_PLCStatusDatas.insert("PumpValue6", CoaterParaType::PLCStatusData(7230, 1));
	m_PLCStatusDatas.insert("PumpValue7", CoaterParaType::PLCStatusData(7231, 1));
	m_PLCStatusDatas.insert("PumpValue8", CoaterParaType::PLCStatusData(7232, 1));
	m_PLCStatusDatas.insert("PumpValue9", CoaterParaType::PLCStatusData(7233, 1));
	m_PLCStatusDatas.insert("PumpValue10", CoaterParaType::PLCStatusData(7234, 1));

	m_PLCStatusDatas.insert("PumpSize1", CoaterParaType::PLCStatusData(7236, 1));
	m_PLCStatusDatas.insert("PumpSize2", CoaterParaType::PLCStatusData(7237, 1));
	m_PLCStatusDatas.insert("PumpSize3", CoaterParaType::PLCStatusData(7238, 1));
	m_PLCStatusDatas.insert("PumpSize4", CoaterParaType::PLCStatusData(7239, 1));
	m_PLCStatusDatas.insert("PumpSize5", CoaterParaType::PLCStatusData(7240, 1));
	m_PLCStatusDatas.insert("PumpSize6", CoaterParaType::PLCStatusData(7241, 1));
	m_PLCStatusDatas.insert("PumpSize7", CoaterParaType::PLCStatusData(7242, 1));
	m_PLCStatusDatas.insert("PumpSize8", CoaterParaType::PLCStatusData(7243, 1));
	m_PLCStatusDatas.insert("PumpSize9", CoaterParaType::PLCStatusData(7244, 1));
	m_PLCStatusDatas.insert("PumpSize10", CoaterParaType::PLCStatusData(7245, 1));

	m_PLCStatusDatas.insert("AlarmStatus", CoaterParaType::PLCStatusData(7247, 1));//报警状态
	m_PLCStatusDatas.insert("CoatingResult", CoaterParaType::PLCStatusData(7248, 1));//生化结果

	//命令地址
	m_PLCCmdDatas.clear();
	m_PLCCmdDatas.insert("Startup", "DM7300");
	m_PLCCmdDatas.insert("Pause", "DM7301");
	m_PLCCmdDatas.insert("Halt", "DM7302");
	m_PLCCmdDatas.insert("Reset", "DM7303");
	m_PLCCmdDatas.insert("Stop", "DM7304");//

	m_PLCCmdDatas.insert("ClearAlarm", "DM7305");
	m_PLCCmdDatas.insert("LoadFinish", "DM7306");
	m_PLCCmdDatas.insert("UnloadFinish", "DM7307");
	m_PLCCmdDatas.insert("SafePos", "DM7308");
	m_PLCCmdDatas.insert("ChipVacuum", "DM7309");

	//m_PLCCmdDatas.insert("InitPump1", "DM7313");
	//m_PLCCmdDatas.insert("InitPump2", "DM7314");
	//m_PLCCmdDatas.insert("InitPump3", "DM7315");
	//m_PLCCmdDatas.insert("InitPump4", "DM7316");
	//m_PLCCmdDatas.insert("InitPump5", "DM7317");
	//m_PLCCmdDatas.insert("InitPump6", "DM7318");
	//m_PLCCmdDatas.insert("InitPump7", "DM7319");
	//m_PLCCmdDatas.insert("InitPump8", "DM7320");
	//m_PLCCmdDatas.insert("InitPump9", "DM7321");
	//m_PLCCmdDatas.insert("InitPump10", "DM7322");
}

void CoaterDriver::ResetPLCValue()
{
	for (auto it = m_PLCStatusDatas.begin(); it != m_PLCStatusDatas.end(); ++it)
	{
		it->Value = 0;
	}
}

void CoaterDriver::SlotConnectStatus(bool bSuccess)
{
	m_Connected = bSuccess;
	emit SignalCoaterConnectStatus(bSuccess);

	if (bSuccess)
	{
		//连接成功后下发一次机械臂安全状态
		SetSafetyStatus(m_bSafePos);
	}
	else
	{
		//断开后清空数值
		ResetPLCValue();
		m_CanStartup = false;
	}
}

void CoaterDriver::GetConnStatus()
{
	emit SignalCoaterConnectStatus(m_Connected);
}

bool CoaterDriver::IsLoadState()
{
	return m_PLCStatusDatas["DeviceMode"].Value == 2 && m_PLCStatusDatas["DeviceStatus"].Value == 2 && m_PLCStatusDatas["LoadStatus"].Value == 1;
}

bool CoaterDriver::IsUnloadState()
{
	return m_PLCStatusDatas["DeviceMode"].Value == 2 && m_PLCStatusDatas["DeviceStatus"].Value == 2 && m_PLCStatusDatas["UnloadStatus"].Value == 1;
}

bool CoaterDriver::SwitchVacuum(bool bOpen)
{
	QString str = "Coater SwitchVacuum: " + QString::number(bOpen);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_Connected)
	{
		return false;
	}

	QString cmd = m_PLCCmdDatas["ChipVacuum"] + (bOpen ? " 1" : " 2");
	return SendWriteCommand(cmd);
}

bool CoaterDriver::SetSafetyStatus(bool bSafety)
{
	QString str = "Coater SetSafetyStatus: " + QString::number(bSafety);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_Connected)
	{
		return false;
	}

	QString cmd = m_PLCCmdDatas["SafePos"] + (bSafety ? " 1" : " 0");
	return SendWriteCommand(cmd);
}

float CoaterDriver::GetVacuumValue()
{
	return IntToFloat(m_PLCStatusDatas["ChipVacuum"].Value);
}

bool CoaterDriver::SetLoadFinish()
{
	BGI_MESSAGE("Coater SetLoadFinish");

	if (!m_Connected)
	{
		return false;
	}

	//ResetLoadStatus();

	QString cmd = m_PLCCmdDatas["LoadFinish"] + " 1";
	return SendWriteCommand(cmd);
}

bool CoaterDriver::SetUnloadFinish()
{
	BGI_MESSAGE("Coater SetUnloadFinish");

	if (!m_Connected)
	{
		return false;
	}

	//ResetUnloadStatus();

	QString cmd = m_PLCCmdDatas["UnloadFinish"] + " 1";
	return SendWriteCommand(cmd);
}

bool CoaterDriver::ResetLoadStatus()
{
	if (!m_Connected)
	{
		return false;
	}

	QString cmd = "DM" + QString::number(m_PLCStatusDatas["LoadStatus"].Addr) + " 0";
	return SendWriteCommand(cmd);
}

bool CoaterDriver::ResetUnloadStatus()
{
	if (!m_Connected)
	{
		return false;
	}

	QString cmd = "DM" + QString::number(m_PLCStatusDatas["UnloadStatus"].Addr) + " 0";
	return SendWriteCommand(cmd);
}

bool CoaterDriver::SendWriteCommand(QString cmd)
{
	m_DataType = PLCDataType::OTHER_TYPE;
	return SendCommand("WR " + cmd, true);
}

bool CoaterDriver::SendTriggerCommand(QString cmdKey)
{
	if (!m_Connected)
	{
		return false;
	}

	QString cmd = cmdKey + " 1";
	bool bRet1 = SendWriteCommand(cmd);

	//触发式命令，下发后延迟20毫秒恢复
	QThread::msleep(20);

	cmd = cmdKey + " 0";
	bool bRet2 = SendWriteCommand(cmd);

	return bRet1 && bRet2;
}

//启动
bool CoaterDriver::Startup()
{
	BGI_MESSAGE("Coater Startup");

	if (!m_CanStartup)
	{
		BGI_WARNING("Coater Startup not ready!");
		return false;
	}

	return SendTriggerCommand(m_PLCCmdDatas["Startup"]);
}

//停止
bool CoaterDriver::Stop()
{
	BGI_MESSAGE("Coater Stop");

	return SendTriggerCommand(m_PLCCmdDatas["Stop"]);
}

//急停
bool CoaterDriver::Halt()
{
	BGI_MESSAGE("Coater Halt");

	return SendTriggerCommand(m_PLCCmdDatas["Halt"]);
}

//暂停
bool CoaterDriver::Pause(bool bPause)
{
	QString str = "Coater Pause: " + QString::number(bPause);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_Connected)
	{
		return false;
	}

	QString cmd = m_PLCCmdDatas["Pause"] + (bPause ? " 1" : " 0");
	return SendWriteCommand(cmd);
}

//复位
bool CoaterDriver::Reset()
{
	BGI_MESSAGE("Coater Reset");

	return SendTriggerCommand(m_PLCCmdDatas["Reset"]);
}

bool CoaterDriver::AlarmReset()
{
	BGI_MESSAGE("Coater AlarmReset");

	return SendTriggerCommand(m_PLCCmdDatas["ClearAlarm"]);
}

void CoaterDriver::ReadCmdData(bool bRead)
{
	m_bReadCommand = bRead;
}

void CoaterDriver::QueryPLCData()
{
	if (m_WaitReply)
	{
		//正等待数据，本次不查询
		//BGI_MESSAGE("WaitReply, skip timer query");
		return;
	}

	//读报警信息
	m_DataType = PLCDataType::ALARM_TYPE;
	//QString cmd = "RDS DM7000.S 100";
	SendCommand(m_QueryAlramStr);

	//读涂布机状态
	m_DataType = PLCDataType::STATUS_TYPE;
	//cmd = "RDS DM7200.S 100";
	SendCommand(m_QueryStatusStr);

	//读上位机写入的数据
	if (m_bReadCommand)
	{
		m_DataType = PLCDataType::COMMAND_TYPE;
		//cmd = "RDS DM7300.S 100";
		SendCommand(m_QueryCommandStr);
	}
}

bool CoaterDriver::SendCommand(QString cmd, bool bLog)
{
	if (bLog)
	{
		QString str = "SendCommand: " + cmd;
		BGI_MESSAGE(str.toStdString().c_str());
	}

	m_SendLock.Acquire();

	ClearBuff();
	m_WaitReply = true;

	//自动加0x0D
	QByteArray buf = (cmd + "\r").toUtf8();
	m_pSocket->Write(buf);

	bool bRet = WaitReplay();
	m_SendLock.Release();

	return bRet;
}

bool CoaterDriver::WaitReplay()
{
	int time = 10;
	int count = 100;
	int i = 0;
	while (i < count)
	{
		QThread::msleep(time);

		if (!m_WaitReply)
		{
			//PLC已经响应
			//BGI_DEBUG("WaitReply done");
			return true;
		}

		++i;
	}

	//超时
	BGI_WARNING("Wait PLC Replay timeout!");
	m_WaitReply = false;
	return false;
}

void CoaterDriver::ClearBuff()
{
	m_BgiLock.Acquire();
	m_RecvBuf.clear();
	m_BgiLock.Release();
}

void CoaterDriver::ExplainBuff(QByteArray buf)
{
	if (!m_WaitReply)
	{
		//没有等待的数据
		return;
	}

	m_BgiLock.Acquire();

	//新来的数据追加到末尾
	m_RecvBuf.append(buf);
	
	QString msg = GetPLCMsg();
	if (!msg.isEmpty())
	{
		//找到了完整的消息
		QStringList datas = msg.split(' ', Qt::SkipEmptyParts);
		QMap<int, int> values;
		switch (m_DataType)
		{
		case ALARM_TYPE:
			values = UpdateData(datas, m_AlarmArray);
			SendNewAlarm(values);
			break;
		case STATUS_TYPE:
			UpdateData(datas, m_StatusArray);
			UpdateStatusData(datas);
			break;
		case COMMAND_TYPE:
			UpdateData(datas, m_CmdArray);
			break;
		default:
			break;
		}
	}

	m_BgiLock.Release();
}

QString CoaterDriver::GetPLCMsg()
{
	int stop = m_RecvBuf.lastIndexOf("\r\n");
	if (stop == -1)
	{
		//还没结束
		return "";
	}
	int start = m_RecvBuf.lastIndexOf("\r\n", stop - 1);
	if (start == -1)
	{
		//只有一个消息，从头开始截取
		start = 0;
	}
	else
	{
		//有多条消息，只取最新一条
		start = start + 2;
	}
	if (stop <= start)
	{
		return "";
	}

	QString msg(m_RecvBuf.mid(start, stop - start));
	//找到完整的消息，清空buf，结束等待
	m_RecvBuf.clear();
	m_WaitReply = false;
	//BGI_DEBUG("Get one msg, reset WaitReply");

	return msg;
}

QMap<int, int> CoaterDriver::UpdateData(const QStringList& data, QVector<int>& saveData)
{
	QMap<int, int> values;
	values.clear();
	bool bChanged = false;

	for (int i = 0; i < data.count(); ++i)
	{
		if (i >= saveData.size())
		{
			break;
		}

		int value = data.at(i).toInt();
		if (saveData.at(i) != value)
		{
			bChanged = true;
			saveData[i] = value;
			values.insert(i, value);
		}
	}

	if (bChanged)
	{
		emit SignalNewData(m_DataType, saveData);
		if (m_DataType == PLCDataType::ALARM_TYPE)
		{
			SendAllAlarm(saveData);
		}
	}

	return values;
}

void CoaterDriver::UpdateStatusData(const QStringList& data)
{
	int startAddr = 7200;  //状态开始地址

	for (auto it = m_PLCStatusDatas.begin(); it != m_PLCStatusDatas.end(); ++it)
	{
		int index = it->Addr - startAddr;
		if (index >= 0 && index + it->Len - 1 < data.size())
		{
			int value = data.at(index).toInt();
			if (it->Len == 2)
			{
				//高位在后
				value = ((data.at(index + 1).toInt() << 16) & 0xFFFF0000) | (value & 0x0000FFFF);
			}
			
			if (it->Value != value)
			{
				it->Value = value;
				PLCStatusChenged(it.key(), it->Addr, it->Value);
			}
		}
	}
}

void CoaterDriver::PLCStatusChenged(QString key, int addr, int value)
{
	if (key == "DeviceStatus")
	{
		emit SignalDeviceStatus(1, value);
	}
	else if (key == "LoadStatus")
	{
		emit SignalDeviceStatus(2, value);
	}
	else if (key == "UnloadStatus")
	{
		emit SignalDeviceStatus(3, value);
	}
	else if (key == "DeviceMode")
	{
		emit SignalDeviceStatus(4, value);
	}
	else if (key == "StartReady")
	{
		m_CanStartup = (value == 1);
		emit SignalDeviceStatus(5, value);
	}
	else if (key == "ResetReady")
	{
		emit SignalDeviceStatus(6, value);
	}
	else if (key == "PETStatus")
	{
		emit SignalDeviceStatus(7, value);
	}
	else if (key == "AlarmStatus")
	{
		emit SignalDeviceStatus(8, value);
	}
	else if (key == "ChipVacuum")
	{
		emit SignalVacuumValue(1, IntToFloat(value));
	}
	else if (key == "PETVacuum")
	{
		emit SignalVacuumValue(2, IntToFloat(value));
	}
	else if (key == "CoatingResult")
	{
		emit SignalCoatingResult(value);
	}
	else if (key == "FlowStep")
	{
		emit SignalNewStep(GetFlowStepString(addr, value));
	}
	else if (key == "ResetStep")
	{
		emit SignalNewStep(GetResetStepString(addr, value));
	}
	else if (key.startsWith("PumpStatus"))
	{
		int index = key.replace("PumpStatus", "").toInt();
		emit SignalPumpStatus(index, value);
	}
	else if (key.startsWith("PumpValue"))
	{
		int index = key.replace("PumpValue", "").toInt();
		emit SignalPumpValue(index, value);
	}
	else if (key.startsWith("PumpSize"))
	{
		int index = key.replace("PumpSize", "").toInt();
		emit SignalPumpSize(index, value);
	}
}

void CoaterDriver::SendNewAlarm(const QMap<int, int>& values)
{
	int startAddr = 7000;  //报警开始地址

	for (auto it = values.begin(); it != values.end(); ++it)
	{
		if (it.value() != 0)
		{
			int addr = it.key() + startAddr;
			QString str = GetAlarmString(addr, it.value());
			if (!str.isEmpty())
			{
				emit SignalNewAlarm(str);
			}
		}
	}
}

void CoaterDriver::SendAllAlarm(const QVector<int>& alarmData)
{
	int startAddr = 7000;  //报警开始地址
	
	QMap<int, QString> alrams;
	for (int i = 0; i < alarmData.count(); ++i)
	{
		if (alarmData[i] != 0)
		{
			int addr = i + startAddr;
			QString str = GetAlarmString(addr, alarmData[i]);
			if (!str.isEmpty())
			{
				alrams.insert(addr, str);
			}
		}
	}
	
	emit SignalAllAlarm(alrams);
}

float CoaterDriver::IntToFloat(int value)
{
	float fValue = *(float*)&value;
	return fValue;
}

QString CoaterDriver::GetAlarmString(int addr, int value)
{
	QString key = QString::number(addr) + "-" + QString::number(value);
	QString str = "";
	DeviceErrorDescription::Instance()->GetCfgValue("CoaterError/" + key, str);
	return str;
}

QString CoaterDriver::GetResetStepString(int addr, int value)
{
	QString key = QString::number(addr) + "-" + QString::number(value);
	QString str = "";
	CoaterStepDescription::Instance()->GetCfgValue("ResetStep/" + key, str);
	return "步" + QString::number(value) + ": " + str;
}

QString CoaterDriver::GetFlowStepString(int addr, int value)
{
	QString key = QString::number(addr) + "-" + QString::number(value);
	QString str = "";
	CoaterStepDescription::Instance()->GetCfgValue("FlowStep/" + key, str);
	return "步" + QString::number(value) + ": " + str;
}

//更新机械臂进入状态
void CoaterDriver::UpdateRobotSafetyStatus(bool bSafety)
{
	if (m_bSafePos != bSafety)
	{
		m_bSafePos = bSafety;
		emit SignalCoaterSafePos(m_bSafePos);
		SetSafetyStatus(m_bSafePos);
	}
}
