/*
* @Abrief 输入参数的接口
* @note 用于输入参数的Agent,并提供便利的访问接口
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _INPUT_PROC_AGENT_HELPER_H_
#define _INPUT_PROC_AGENT_HELPER_H_

#include "BgiProcId.h"
#include <vector>
#include <map>


class ParaAgent;
class ProcParaManager;
class ParaBase;
class GeneralInputParaAgent;
class RobotInputParaAgent;
class CoaterInputParaAgent;
class ImagerInputParaAgent;

class InputProcAgentHelper
{
public:
    InputProcAgentHelper(ProcParaManager* manager);
    void Reset();
    ParaAgent* GetAgent(BgiProc::ESystemType system_type, BgiProc::ESystemId system_id);
    ParaBase* GetParameter(BgiProc::ESystemType system_type, BgiProc::ESystemId system_id, int para_id);

    GeneralInputParaAgent& General();
    RobotInputParaAgent& Robot(BgiProc::ESystemId sys_id);
    CoaterInputParaAgent& Coater(BgiProc::ESystemId sys_id);
    ImagerInputParaAgent& Imager(BgiProc::ESystemId sys_id);

    static std::vector<ParaAgent*> BuildAgent();

private:
    ProcParaManager* m_Manager;

    GeneralInputParaAgent* m_General;
    //机器手,一个系统
    RobotInputParaAgent* m_Robot[1];
    //涂布机,一个系统
    CoaterInputParaAgent* m_Coater[1];
    //光机,一个系统
    ImagerInputParaAgent* m_Imager[1];
};


#endif
