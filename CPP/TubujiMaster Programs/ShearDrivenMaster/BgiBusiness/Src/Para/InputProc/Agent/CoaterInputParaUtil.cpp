#include "CoaterInputParaUtil.h"
#include "BgiLogger.h"
#include "ParaCmd.h"
#include "BgiProcId.h"
#include "InputProcId.h"
#include "ParaEngine.h"
#include "GeneralInputParaAgent.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "CoaterInputParaAgent.h"


void CoaterInputParaUtil::SetSortingStatus(GeneralParaType::WorkStatus value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = InputProc::Coater::Start;
	para_cmd.ProcType = BgiProc::EProcType::InputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::CoaterType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(GeneralParaType::WorkStatus);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

//获取
void CoaterInputParaUtil::GetSortingStatus(GeneralParaType::WorkStatus& status)
{
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	CoaterInputParaAgent& agent = input_helper.Coater(BgiProc::ESystemId::SystemA);
	//status = agent.Start().Value();
	GeneralParaType::WorkStatus tempstatus;
	status = tempstatus;
}
