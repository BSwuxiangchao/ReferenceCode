#include "CoaterOutputParaControl.h"
#include "BgiLogger.h"
#include "OutputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "OutputProcAgentHelper.h"
#include "CoaterOutputParaAgent.h"

CoaterOutputParaControl::CoaterOutputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)OutputProc::Coater::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
	
}


CoaterOutputParaControl::~CoaterOutputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}


int CoaterOutputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId CoaterOutputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType CoaterOutputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::CoaterType;
}

BgiProc::EProcType CoaterOutputParaControl::GetProcType()
{
	return BgiProc::EProcType::OutputProcType;
}


ParaBase* CoaterOutputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	OutputProcAgentHelper output_helper(paraManager);
	CoaterOutputParaAgent& agent = output_helper.Coater(system_id);
	return agent.GetParameter(m_iParaId);
}
