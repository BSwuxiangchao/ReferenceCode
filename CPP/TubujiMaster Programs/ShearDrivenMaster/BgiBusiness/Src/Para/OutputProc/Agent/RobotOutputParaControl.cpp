#include "RobotOutputParaControl.h"
#include "BgiLogger.h"
#include "OutputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "OutputProcAgentHelper.h"
#include "RobotOutputParaAgent.h"

RobotOutputParaControl::RobotOutputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)OutputProc::Robot::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
	
}


RobotOutputParaControl::~RobotOutputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}


int RobotOutputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId RobotOutputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType RobotOutputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::RobotType;
}

BgiProc::EProcType RobotOutputParaControl::GetProcType()
{
	return BgiProc::EProcType::OutputProcType;
}


ParaBase* RobotOutputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	OutputProcAgentHelper output_helper(paraManager);
	RobotOutputParaAgent& agent = output_helper.Robot(system_id);
	return agent.GetParameter(m_iParaId);
}
