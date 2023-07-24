#include "ImagerControlMgrIPCCallbackI.h"
#include "ImagerDriver.h"
#include "ImagerControlMgrIPCClient.h"

ImagerControlMgrIPCCallbackI::ImagerControlMgrIPCCallbackI(ImagerDriver* imagerDriver)
	: m_ImagerDriver(imagerDriver)
{
}

ImagerControlMgrIPCCallbackI::~ImagerControlMgrIPCCallbackI()
{}

void ImagerControlMgrIPCCallbackI::SendImagerError(ImagerErrorEnum error, const ::Ice::Current& current)
{
	if (error > 0)
	{
		m_ImagerDriver->RecvImagerError((int)error);
	}
}

void ImagerControlMgrIPCCallbackI::SendImagerRunInfo(const ImagerRunInfo& info, const ::Ice::Current& current)
{
	m_ImagerDriver->RecvImagerRunInfo(SwitchImagerRunData(info));
}

void ImagerControlMgrIPCCallbackI::SendImagerTaskEnd(const ImagerSetupInfo& info, ImagerErrorEnum error, const ::Ice::Current& current)
{
	//光机每次拍照结束，会调用两次，第一次ImagerCycle = 0，第二次是实际的ImagerCycle
	if (info.ImagerCycle > 0)
	{
		m_ImagerDriver->RecvImagerTaskEnd(ImagerControlMgrIPCClient::SwitchImagerSetupData(info), (int)error);
	}
}

void ImagerControlMgrIPCCallbackI::SendData(const ::std::string& key, const ::std::string& value, const ::Ice::Current& current)
{
	if (key == "Vacuum")
	{
		m_ImagerDriver->RecvImagerVacuumValue(QString::fromStdString(value));
	}
	else
	{
		m_ImagerDriver->RecvImagerData(QString::fromStdString(key), QString::fromStdString(value));
	}
}

int  ImagerControlMgrIPCCallbackI::SendHeartbeat(::Ice::Int count, const ::Ice::Current& current)
{
	m_ImagerDriver->Heartbeat();
	return count + 1;
}

void ImagerControlMgrIPCCallbackI::SendStatus(BasecallConnectState StateBasecall, ImagerServerState StateServer, ImagerState StateImager, const ::Ice::Current& current)
{
	m_ImagerDriver->RecvImagerStatus((int)StateBasecall, (int)StateServer, (int)StateImager);
}

void ImagerControlMgrIPCCallbackI::AddClient(const ::Ice::Identity& ident, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendScriptName(const ::std::string& scriptName, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendActionName(const ::std::string& actionName, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendFlowcellBarcode(const ::std::string& flowcellBarcode, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendPositionNumber(::Ice::Int cycleNum, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendPcntDone(::Ice::Int pctDone, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendScriptStatus(ScriptStateEnum curState, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendScriptMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendStageMessage(const ::std::string& msgType, const ::std::string& message, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendQCValue(const QCDataValue& qcValue, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendQCValues(const QCDataValues& qcValues, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendAlarmMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendLogMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendRemainingTime(::Ice::Double remainingTimeMinute, const ::Ice::Current& current)
{
}

void ImagerControlMgrIPCCallbackI::SendDataList(const DataList& datas, const ::Ice::Current& current)
{
}

ImagerParaType::ImagerRunPara ImagerControlMgrIPCCallbackI::SwitchImagerRunData(const BGI::RPC::ImagerRunInfo& dataOri)
{
    ImagerParaType::ImagerRunPara data;

    data.SlideID = QString::fromStdString(dataOri.SlideID);
    data.CurrentRow = dataOri.CurrentRow;
    data.CurrentCol = dataOri.CurrentCol;
    data.CurrentCycle = dataOri.CurrentCycle;
    data.CurrentLane = dataOri.CurrentLane;
    data.StartRow = dataOri.StartRow;
    data.EndRow = dataOri.EndRow;
    data.StartCol = dataOri.StartCol;
    data.EndCol = dataOri.EndCol;
    data.TotalLane = dataOri.TotalLane;
    data.ScanDirect = dataOri.ScanDirect;

    return data;
}
