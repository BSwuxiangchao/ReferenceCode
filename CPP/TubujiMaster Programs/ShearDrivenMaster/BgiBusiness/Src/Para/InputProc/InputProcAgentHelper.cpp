#include "InputProcAgentHelper.h"
#include "BgiLogger.h"
#include "ProcParaManager.h"
#include "Parameter.h"
#include "GeneralInputParaAgent.h"
#include "RobotInputParaAgent.h"
#include "CoaterInputParaAgent.h"
#include "ImagerInputParaAgent.h"
#include <algorithm>

using namespace std;

enum AgentType {
    At_Gen = 0,
    At_Robot,
    At_Coater,
    At_Imager,
    At_Num,
};

InputProcAgentHelper::InputProcAgentHelper(ProcParaManager* manager)
    :m_Manager(manager)
{
    m_General = NULL;
    m_Robot[0] = NULL;
    m_Coater[0] = NULL;
    m_Imager[0] = NULL;
}

void InputProcAgentHelper::Reset()
{
    const vector<ParaAgent*>& agent = m_Manager->GetInputProcAgent();
    vector<ParaAgent*>::const_iterator it = agent.begin();
    vector<ParaAgent*>::const_iterator it_end = agent.end();
    for (; it != it_end; ++it)
        (*it)->ResetPara();
}

ParaAgent* InputProcAgentHelper::GetAgent(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id)
{
    ParaAgent* agent = NULL;
    const vector<ParaAgent*>& agent_array = m_Manager->GetInputProcAgent();
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

ParaBase* InputProcAgentHelper::GetParameter(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id, int para_id)
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

GeneralInputParaAgent& InputProcAgentHelper::General()
{
    if (m_General == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetInputProcAgent();
        m_General = dynamic_cast<GeneralInputParaAgent*>(agent[At_Gen]);
        BGI_ASSERT(m_General != NULL);
    }
    return *m_General;
}

RobotInputParaAgent& InputProcAgentHelper::Robot(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Robot[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetInputProcAgent();
        //m_Robot[idx] = (idx == 0) ? dynamic_cast<SynthesisInputParaAgent*>(agent[At_Robot]) : dynamic_cast<SynthesisInputParaAgent*>(agent[At_SynthesisB]);
        m_Robot[idx] = dynamic_cast<RobotInputParaAgent*>(agent[At_Robot]);
        BGI_ASSERT(m_Robot[idx] != NULL);
    }
    return *m_Robot[idx];
}

CoaterInputParaAgent& InputProcAgentHelper::Coater(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Coater[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetInputProcAgent();
        //m_Robot[idx] = (idx == 0) ? dynamic_cast<SynthesisInputParaAgent*>(agent[At_Robot]) : dynamic_cast<SynthesisInputParaAgent*>(agent[At_SynthesisB]);
        m_Coater[idx] = dynamic_cast<CoaterInputParaAgent*>(agent[At_Coater]);
        BGI_ASSERT(m_Coater[idx] != NULL);
    }
    return *m_Coater[idx];
}

ImagerInputParaAgent& InputProcAgentHelper::Imager(BgiProc::ESystemId sys_id)
{
    int idx = SystemIdToIdx(sys_id);
    if (m_Imager[idx] == NULL)
    {
        const vector<ParaAgent*>& agent = m_Manager->GetInputProcAgent();
        //m_Robot[idx] = (idx == 0) ? dynamic_cast<SynthesisInputParaAgent*>(agent[At_Robot]) : dynamic_cast<SynthesisInputParaAgent*>(agent[At_SynthesisB]);
        m_Imager[idx] = dynamic_cast<ImagerInputParaAgent*>(agent[At_Imager]);
        BGI_ASSERT(m_Imager[idx] != NULL);
    }
    return *m_Imager[idx];
}

vector<ParaAgent*> InputProcAgentHelper::BuildAgent()
{
    ParaAgent* agent[At_Num];
    agent[At_Gen] = new GeneralInputParaAgent();
    agent[At_Robot] = new RobotInputParaAgent(BgiProc::SystemA);
    agent[At_Coater] = new CoaterInputParaAgent(BgiProc::SystemA);
    agent[At_Imager] = new ImagerInputParaAgent(BgiProc::SystemA);
    //agent[At_SynthesisB] = new SynthesisInputParaAgent(BgiProc::SystemB);

    vector<ParaAgent*> result;
    for (int i = 0; i < At_Num; i++)
        result.push_back(agent[i]);

    return result;
}

