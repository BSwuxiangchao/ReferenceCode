#include "RobotInputParaControl.h"
#include "BgiLogger.h"
#include "InputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "RobotInputParaAgent.h"
#include "RobotParaType.h"

RobotInputParaControl::RobotInputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)InputProc::Robot::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
}


RobotInputParaControl::~RobotInputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}

int RobotInputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId RobotInputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType RobotInputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::RobotType;
}

BgiProc::EProcType RobotInputParaControl::GetProcType()
{
	return BgiProc::EProcType::InputProcType;
}


ParaBase* RobotInputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	RobotInputParaAgent& agent = input_helper.Robot(system_id);

	ParaBase* base = agent.GetParameter(m_iParaId);

	return base;
}