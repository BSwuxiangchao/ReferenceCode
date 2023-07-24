#include "ImagerInputParaControl.h"
#include "BgiLogger.h"
#include "InputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "ImagerInputParaAgent.h"
#include "ImagerParaType.h"

ImagerInputParaControl::ImagerInputParaControl(QString systemId, QString ParaId)
	:ParaControl()
{
	m_SystemId = systemId;
	m_strParaId = ParaId;
	m_iParaId = (int)InputProc::Imager::GetParaId(ParaId);

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->AddObserver(this, PM_VALUE | PM_STATUS);
	}
}


ImagerInputParaControl::~ImagerInputParaControl()
{
	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para->DelObserver(this);
	}
}

int ImagerInputParaControl::GetParaId()
{
	return m_iParaId;
}

BgiProc::ESystemId ImagerInputParaControl::GetSystemId()
{
	return BgiProc::GetSystemId(m_SystemId);
}

BgiProc::ESystemType ImagerInputParaControl::GetSystemType()
{
	return BgiProc::ESystemType::ImagerType;
}

BgiProc::EProcType ImagerInputParaControl::GetProcType()
{
	return BgiProc::EProcType::InputProcType;
}


ParaBase* ImagerInputParaControl::GetParaBase()
{
	BgiProc::ESystemId system_id;
	system_id = BgiProc::GetSystemId(m_SystemId);
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	ImagerInputParaAgent& agent = input_helper.Imager(system_id);

	ParaBase* base = agent.GetParameter(m_iParaId);

	return base;
}