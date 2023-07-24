#include "ParaControl.h"
#include "BgiLogger.h"
#include "InputProcId.h"
#include "OutputProcId.h"
#include "ParaCmd.h"
#include "ParaEngine.h"
#include "BgiProcId.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "OutputProcAgentHelper.h"

#include "BgiLogger.h"

ParaControl::ParaControl()
{
	m_ControlObserver.clear();
}


ParaControl::~ParaControl()
{
	
}

void ParaControl::AddObserver(ParaObserver* observer)
{
	BGI_ASSERT(std::find(m_ControlObserver.begin(), m_ControlObserver.end(), observer) == m_ControlObserver.end());
	m_ControlObserver.push_back(observer);
}

void ParaControl::DelObserver(ParaObserver* observer)
{
	std::vector<ParaObserver*>::iterator it = find(m_ControlObserver.begin(), m_ControlObserver.end(), observer);
	assert(it != m_ControlObserver.end());
	m_ControlObserver.erase(it);
}

void ParaControl::Update()
{
	std::vector<ParaObserver*>::iterator it = m_ControlObserver.begin();
	std::vector<ParaObserver*>::iterator it_end = m_ControlObserver.end();
	for (; it != it_end; ++it)
		(*it)->Update();
}

void ParaControl::SetValue(const void* value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = GetParaId();
	para_cmd.ProcType = GetProcType();
	para_cmd.SystemType = GetSystemType();
	para_cmd.SystemId = GetSystemId();
	para_cmd.Value = value;

	ParaBase* para = GetParaBase();
	if (para != NULL)
	{
		para_cmd.ValueType = &para->GetTypeInfo();
	}

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

const void* ParaControl::GetValue()
{
	ParaBase* para = GetParaBase();

	if (para != NULL)
	{
		return para->GetValue(para->GetTypeInfo(), 0);
	}
	return NULL;
}
