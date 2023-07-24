#include "ImagerInputParaUtil.h"
#include "BgiLogger.h"
#include "ParaCmd.h"
#include "BgiProcId.h"
#include "InputProcId.h"
#include "ParaEngine.h"
#include "GeneralInputParaAgent.h"
#include "ProcParaManager.h"
#include "InputProcAgentHelper.h"
#include "ImagerInputParaAgent.h"

//设置

void ImagerInputParaUtil::SetTransportStatus(GeneralParaType::WorkStatus value)
{
	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = InputProc::Imager::Start;
	para_cmd.ProcType = BgiProc::EProcType::InputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::ImagerType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &value;
	para_cmd.ValueType = &typeid(GeneralParaType::WorkStatus);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}

//获取
void ImagerInputParaUtil::GetTransportStatus(GeneralParaType::WorkStatus& status)
{
	ProcParaManager* paraManager = ParaEngine::Instance()->GetProcParaManager();
	InputProcAgentHelper input_helper(paraManager);
	ImagerInputParaAgent& agent = input_helper.Imager(BgiProc::ESystemId::SystemA);
	status = agent.WorkStatus().Value();
}

