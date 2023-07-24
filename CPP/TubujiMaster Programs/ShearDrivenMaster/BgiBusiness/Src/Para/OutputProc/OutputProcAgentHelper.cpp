#include "OutputProcAgentHelper.h"
#include "BgiLogger.h"
#include "ProcParaManager.h"
#include "GeneralOutputParaAgent.h"
#include "RobotOutputParaAgent.h"
#include "CoaterOutputParaAgent.h"
#include "ImagerOutputParaAgent.h"
#include "Parameter.h"

#include <algorithm>

using namespace std;

enum AgentType {
    At_Gen = 0,
    At_Robot,
    At_Coater,
    At_Imager,
    At_Num,
};

OutputProcAgentHelper::OutputProcAgentHelper(ProcParaManager* manager)
    :m_Manager(manager)
{
    m_General = NULL;
    m_Robot[0] = NULL;
    m_Coater[0] = NULL;
    m_Imager[0] = NULL;
}

void OutputProcAgentHelper::Reset()
{
    const vector<ParaAgent*>& agent = m_Manager->GetOutputProcAgent();
    vector<ParaAgent*>::const_iterator it = agent.begin();
    vector<ParaAgent*>::const_iterator it_end = agent.end();
    for (; it != it_end; ++it)
        (*it)->ResetPara();
}

ParaAgent* OutputProcAgentHelper::GetAgent(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id)
{
    ParaAgent* agent = NULL;
    const vector<ParaAgent*>& agent_array = m_Manager->GetOutputProcAgent();
    vector<ParaAgent*>::const_iterator it = agent_array.begin();
    vector<ParaAgent*>::const_iterator it_end = agent_array.end();
    for (; it != it_end; ++it)
    {
        if (((*it)->GetSystemType() == system_type) && ((*it)->GetSystemId() == sys_id))
        {
            agent = *it;
            break;
        }
    }
    return agent;
}

ParaBase* OutputProcAgentHelper::GetParameter(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id, int para_id)
{
    ParaAgent* agent = GetAgent(system_type, sys_id);
    if (agent != NULL)
        return agent->GetParameter(para_id);
    else
        return NULL;
}

static int SystemIdToIdx(BgiProc::ESystemId sys_id)
{
    int idx = -1;
    switch (sys_id)
    {
    case BgiProc::SystemA:
        idx = 0;
        break;
   
    default:
        BGI_ASSERT(false);
        break;
    }
    return idx;
}

GeneralOutputParaAgent& OutputProcAgentHelper::General()
{
    if (m_General == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetOutputProcAgent();
        m_General = dynamic_cast<GeneralOutputParaAgent*>(agent[At_Gen]);
        BGI_ASSERT(m_General != NULL);
    }
    return *m_General;
}

RobotOutputParaAgent& OutputProcAgentHelper::Robot(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Robot[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetOutputProcAgent();
        //m_Robot[idx] = (idx == 0) ? dynamic_cast<SynthesisOutputParaAgent*>(agent[At_SynthesisA]) : dynamic_cast<SynthesisOutputParaAgent*>(agent[At_SynthesisB]);
        m_Robot[idx] = dynamic_cast<RobotOutputParaAgent*>(agent[At_Robot]);
        BGI_ASSERT(m_Robot[idx] != NULL);
    }
    return *m_Robot[idx];
}

CoaterOutputParaAgent& OutputProcAgentHelper::Coater(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Coater[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetOutputProcAgent();
        m_Coater[idx] = dynamic_cast<CoaterOutputParaAgent*>(agent[At_Coater]);
        BGI_ASSERT(m_Coater[idx] != NULL);
    }
    return *m_Coater[idx];
}

ImagerOutputParaAgent& OutputProcAgentHelper::Imager(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Imager[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetOutputProcAgent();
        m_Imager[idx] = dynamic_cast<ImagerOutputParaAgent*>(agent[At_Imager]);
        BGI_ASSERT(m_Imager[idx] != NULL);
    }
    return *m_Imager[idx];
}

vector<ParaAgent*> OutputProcAgentHelper::BuildAgent()
{
    ParaAgent* agent[At_Num];
    agent[At_Gen] = new GeneralOutputParaAgent();
    agent[At_Robot] = new RobotOutputParaAgent(BgiProc::SystemA);
    agent[At_Coater] = new CoaterOutputParaAgent(BgiProc::SystemA);
    agent[At_Imager] = new ImagerOutputParaAgent(BgiProc::SystemA);

    vector<ParaAgent*> result;
    for (int i = 0; i < At_Num; i++)
        result.push_back(agent[i]);

    return result;
}

