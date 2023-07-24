#include "ProcParaManager.h"
#include "ParaAgent.h"
#include "BgiLogger.h"
#include <map>

ProcParaManager::ProcParaManager(std::vector<ParaAgent*> input_proc_agent,
    std::vector<ParaAgent*> output_proc_agent
)
    : ParaManager(1)
    , m_InputProcAgent(input_proc_agent)
    , m_OutputProcAgent(output_proc_agent)
{
    //创建参数
    AddAgent(m_InputProcAgent);
    AddAgent(m_OutputProcAgent);

    //设置参数的初始值
    BeginChange();
    ResetParameter();
    EndChange();
}

ProcParaManager::~ProcParaManager()
{
    DelAgent(m_InputProcAgent);
    DelAgent(m_OutputProcAgent);
}

void ProcParaManager::ResetParameter()
{
    ResetAgent(m_InputProcAgent);
    ResetAgent(m_OutputProcAgent);

    //将所有参数都Trigger一次
    std::map<int, Parameter*>::iterator it = m_ParameterMap.begin();
    std::map<int, Parameter*>::iterator it_end = m_ParameterMap.end();
    for (; it != it_end; ++it)
        Trigger(it->first);
}

void ProcParaManager::ResetAgent(std::vector<ParaAgent*>& agent)
{
    std::vector<ParaAgent*>::iterator it = agent.begin();
    std::vector<ParaAgent*>::iterator it_end = agent.end();
    for (; it != it_end; ++it)
    {
        BGI_ASSERT((*it) != NULL);
        (*it)->ResetPara();
    }
}

void ProcParaManager::AddAgent(std::vector<ParaAgent*>& agent)
{
    std::vector<ParaAgent*>::iterator it = agent.begin();
    std::vector<ParaAgent*>::iterator it_end = agent.end();
    for (; it != it_end; ++it)
    {
        BGI_ASSERT((*it) != NULL);
        (*it)->SetParaManager(this);
        (*it)->CreatePara();
    }
}

void ProcParaManager::DelAgent(std::vector<ParaAgent*>& agent)
{
    std::vector<ParaAgent*>::iterator it = agent.begin();
    std::vector<ParaAgent*>::iterator it_end = agent.end();
    for (; it != it_end; ++it)
    {
        BGI_ASSERT((*it) != NULL);
        delete (*it);
        (*it) = NULL;
    }
}

const std::vector<ParaAgent*>& ProcParaManager::GetInputProcAgent() const
{
    return m_InputProcAgent;
}

const std::vector<ParaAgent*>& ProcParaManager::GetOutputProcAgent() const
{
    return m_OutputProcAgent;
}

