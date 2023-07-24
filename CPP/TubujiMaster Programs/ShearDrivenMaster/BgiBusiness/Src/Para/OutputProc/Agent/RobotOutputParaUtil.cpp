#include "RobotOutputParaUtil.h"
#include <qcoreapplication.h>
#include <qthread.h>
#include "BgiLogger.h"
#include "ParaCmd.h"
#include "BgiProcId.h"
#include "OutputProcId.h"
#include "ParaEngine.h"
#include "RobotOutputParaAgent.h"
#include "ProcParaManager.h"
#include "OutputProcAgentHelper.h"

QVector<RobotParaType::RobotPara> RobotOutputParaUtil::m_vecRobotParaStatisSeq;
int RobotOutputParaUtil::m_iAtRobotSafety = 0;
QString RobotOutputParaUtil::m_strScannerInfo = "";

RobotOutputParaUtil::RobotOutputParaUtil()
{
	m_vecRobotParaStatisSeq.clear();
}

void RobotOutputParaUtil::SetRobotPos(RobotParaType::RobotPoint value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = OutputProc::Robot::RobotPoint;
	para_cmd.ProcType = BgiProc::EProcType::OutputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::RobotType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(RobotParaType::RobotPoint);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

void RobotOutputParaUtil::SetRobotStatus(RobotParaType::RobotStatus value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = OutputProc::Robot::WorkStatus;
	para_cmd.ProcType = BgiProc::EProcType::OutputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::RobotType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(RobotParaType::RobotStatus);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

//设置参数
void RobotOutputParaUtil::SetRobotPara(RobotParaType::RobotPara value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = OutputProc::Robot::ParaOpt;
	para_cmd.ProcType = BgiProc::EProcType::OutputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::RobotType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(RobotParaType::RobotPara);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

void RobotOutputParaUtil::SetRobotSafetyState(int iState)
{
	m_iAtRobotSafety = iState;
}

int RobotOutputParaUtil::GetRobotSafetyState()
{
	return m_iAtRobotSafety;
}

void RobotOutputParaUtil::SetScannerInfo(QString strInfo)
{
	m_strScannerInfo = strInfo;
}

void RobotOutputParaUtil::ApplicationSleep(int mSec)
{
	QThread::msleep(mSec);
	/*while (mSec)
	{
		QCoreApplication::processEvents();
		QThread::msleep(1);
		mSec--;
	}*/
}

bool RobotOutputParaUtil::GetScannerInfo(QString& strInfo)
{
	int iCount = 0;
	int iTotalCount = 20;
	while (iCount <iTotalCount)
	{
		if (m_strScannerInfo != "" && m_strScannerInfo != "BR")
		{
			strInfo = m_strScannerInfo;
			m_strScannerInfo = "";
			return true;
		}
		if (m_strScannerInfo == "BR")
		{
			strInfo = gc_RescanTip;
			m_strScannerInfo = "";
			return false;
		}
		ApplicationSleep(50);
		iCount++;
	}
	strInfo = gc_ScanErrTip;
	m_strScannerInfo = "";
	return false;
}
