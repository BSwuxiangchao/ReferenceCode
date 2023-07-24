#include "ImagerOutputParaControl.h"
#include "BgiLogger.h"
#include "OutputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "OutputProcAgentHelper.h"
#include "ImagerOutputParaAgent.h"

ImagerOutputParaControl::ImagerOutputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)OutputProc::Imager::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
	
}


ImagerOutputParaControl::~ImagerOutputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}


int ImagerOutputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId ImagerOutputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType ImagerOutputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::ImagerType;
}

BgiProc::EProcType ImagerOutputParaControl::GetProcType()
{
	return BgiProc::EProcType::OutputProcType;
}


ParaBase* ImagerOutputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	OutputProcAgentHelper output_helper(paraManager);
	ImagerOutputParaAgent& agent = output_helper.Imager(system_id);
	return agent.GetParameter(m_iParaId);
}
