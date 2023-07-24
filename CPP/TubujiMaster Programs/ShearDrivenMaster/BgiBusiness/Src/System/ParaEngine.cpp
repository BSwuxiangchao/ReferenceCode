#include "ParaEngine.h"
#include "BgiLogger.h"
#include "ProcParaManager.h"
#include "ParaCmd.h"
#include "ParaAgent.h"
#include "InputProcAgentHelper.h"
#include "OutputProcAgentHelper.h"
#include "ParaBase.h"

QMutex ParaEngine::m_Mutex;
QSharedPointer<ParaEngine> ParaEngine::m_pInstance = NULL;

ParaEngine::ParaEngine()
{
    Initialize();
}

ParaEngine::~ParaEngine()
{

    if(m_ProcParaManager != NULL)
    {
        delete m_ProcParaManager;
        m_ProcParaManager = NULL;
    }
}

void ParaEngine::Initialize()
{
    //初始化处理参数管理
    std::vector<ParaAgent *> input_proc_agent = InputProcAgentHelper::BuildAgent();
    std::vector<ParaAgent*> output_proc_agent = OutputProcAgentHelper::BuildAgent();
    m_ProcParaManager = new ProcParaManager(input_proc_agent, output_proc_agent);

}



ProcParaManager* ParaEngine::GetProcParaManager()
{
    return m_ProcParaManager;
}


bool ParaEngine::ParaCommand(const ParaCmd& para_cmd)
{
    std::vector<ParaCmd> para_cmds;
    para_cmds.push_back(para_cmd);
    return ParaCommand(para_cmds);
}

bool ParaEngine::ParaCommand(const std::vector<ParaCmd>& para_cmds)
{
    bool have_input_proc_cmd = false;
    bool result = true;

    BeginChange();

    //执行命令
    std::vector<ParaCmd>::const_iterator it = para_cmds.begin();
    std::vector<ParaCmd>::const_iterator it_end = para_cmds.end();
    for(; it != it_end; ++it)
    {
        if((*it).ProcType == BgiProc::InputProcType)
            have_input_proc_cmd = true;

        bool cmd_result = ExecuteCommand(*it);
        result = (result && cmd_result);
    }

    EndChange();

    if(have_input_proc_cmd)
        Output();

    return result;
}


bool ParaEngine::ExecuteCommand(const ParaCmd& para_cmd)
{
    bool result = false;
    if (para_cmd.ProcType == BgiProc::InputProcType)
    {
        InputProcAgentHelper helper(m_ProcParaManager);

        ParaBase* parameter = helper.GetParameter(para_cmd.SystemType, para_cmd.SystemId, para_cmd.ParaEnum);
        result = DoExecuteCommand(parameter, para_cmd);
    }
    else if (para_cmd.ProcType == BgiProc::OutputProcType)
    {
        OutputProcAgentHelper helper(m_ProcParaManager);

        ParaBase* parameter = helper.GetParameter(para_cmd.SystemType, para_cmd.SystemId, para_cmd.ParaEnum);
        result = DoExecuteCommand(parameter, para_cmd);
    }

    return result;
}



bool ParaEngine::DoExecuteCommand(ParaBase* parameter, const ParaCmd& para_cmd)
{
    BGI_ASSERT(parameter != NULL);

    bool result;
    switch (para_cmd.Type)
    {
    case ParaCmd::IncLevel:
        parameter->AddLevel(para_cmd.Level);
        result = true;
        break;
    case ParaCmd::DecLevel:
        parameter->AddLevel(-para_cmd.Level);
        result = true;
        break;
    case ParaCmd::SetLevel:
        parameter->Level(para_cmd.Level);
        result = true;
        break;
    case ParaCmd::SetValue:
        parameter->SetValue(para_cmd.Value, *para_cmd.ValueType);
        result = true;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

void ParaEngine::BeginChange()
{
    m_ProcParaManager->BeginChange();
}

void ParaEngine::EndChange()
{
    m_ProcParaManager->EndChange();
}

void ParaEngine::Output()
{
    //ImagingDevice* dev = m_ImagingEnviron->GetImagingDevice();
    //dev->Output(this);
}

