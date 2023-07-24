#include "CoaterInputParaControl.h"
#include "BgiLogger.h"
#include "InputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "CoaterInputParaAgent.h"
#include "CoaterParaType.h"

CoaterInputParaControl::CoaterInputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)InputProc::Coater::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
}


CoaterInputParaControl::~CoaterInputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}

int CoaterInputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId CoaterInputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType CoaterInputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::CoaterType;
}

BgiProc::EProcType CoaterInputParaControl::GetProcType()
{
	return BgiProc::EProcType::InputProcType;
}


ParaBase* CoaterInputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	CoaterInputParaAgent& agent = input_helper.Coater(system_id);

	ParaBase* base = agent.GetParameter(m_iParaId);

	return base;
}