#include "GeneralInputParaControl.h"
#include "BgiLogger.h"
#include "InputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "GeneralInputParaAgent.h"

GeneralInputParaControl::GeneralInputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)InputProc::Gen::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
}


GeneralInputParaControl::~GeneralInputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}

int GeneralInputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId GeneralInputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType GeneralInputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::GeneralType;
}

BgiProc::EProcType GeneralInputParaControl::GetProcType()
{
	return BgiProc::EProcType::InputProcType;
}


ParaBase* GeneralInputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	GeneralInputParaAgent& agent = input_helper.General();

	ParaBase* base = agent.GetParameter(m_iParaId);

	return base;
}