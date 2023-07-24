/*
* @Abrief 输出参数的接口
* @note 用于输出参数的Agent,并提供便利的访问接口
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _OUTPUT_PROC_AGENT_HELPER_H_
#define _OUTPUT_PROC_AGENT_HELPER_H_

#include "BgiProcId.h"
#include <vector>
#include <map>


class ParaAgent;
class ProcParaManager;
class ParaBase;
class GeneralOutputParaAgent;
class RobotOutputParaAgent;
class CoaterOutputParaAgent;
class ImagerOutputParaAgent;

class OutputProcAgentHelper
{
public:
    OutputProcAgentHelper(ProcParaManager* manager);
    void Reset();
    ParaAgent* GetAgent(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id);
    ParaBase* GetParameter(BgiProc::ESystemType system_type, BgiProc::ESystemId sys_id, int para_id);

    GeneralOutputParaAgent& General();
    RobotOutputParaAgent& Robot(BgiProc::ESystemId sys_id);
    CoaterOutputParaAgent& Coater(BgiProc::ESystemId sys_id);
    ImagerOutputParaAgent& Imager(BgiProc::ESystemId sys_id);

    static std::vector<ParaAgent*> BuildAgent();

private:
    ProcParaManager* m_Manager;

    GeneralOutputParaAgent* m_General;
    RobotOutputParaAgent* m_Robot[1];
    CoaterOutputParaAgent* m_Coater[1];
    ImagerOutputParaAgent* m_Imager[1];
};


#endif
