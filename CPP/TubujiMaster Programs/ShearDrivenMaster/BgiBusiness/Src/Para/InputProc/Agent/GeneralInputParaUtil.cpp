#include "GeneralInputParaUtil.h"
#include "BgiLogger.h"
#include "ParaCmd.h"
#include "BgiProcId.h"
#include "InputProcId.h"
#include "ParaEngine.h"
#include "GeneralInputParaAgent.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"

void GeneralInputParaUtil::SetSequencingTableDesc(QVector<GeneralParaType::SequencingTableDesc> value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = InputProc::Gen::SequencingDesc;
	para_cmd.ProcType = BgiProc::EProcType::InputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::GeneralType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(QVector<GeneralParaType::SequencingTableDesc>);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

void GeneralInputParaUtil::GetSequencingTableDesc(QVector<GeneralParaType::SequencingTableDesc>& value)
{
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	GeneralInputParaAgent& agent = input_helper.General();
	value = agent.SequencingTableDescVec().Value();
}
