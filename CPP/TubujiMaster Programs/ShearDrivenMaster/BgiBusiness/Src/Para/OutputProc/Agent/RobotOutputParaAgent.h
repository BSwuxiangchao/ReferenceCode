/*
* @Abrief 输出参数的代理类
* @note 访问输出参数的代理类
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _ROBOT_OUTPUT_PARA_AGENT_H_
#define _ROBOT_OUTPUT_PARA_AGENT_H_

#include "ParaAgent.h"
#include "Parameter.h"
#include "OutputProcId.h"
#include "BgiProcId.h"
#include "RobotParaType.h"

class RobotOutputParaAgent : public ParaAgent
{
public:
    RobotOutputParaAgent(BgiProc::ESystemId sys_id);
    ~RobotOutputParaAgent();

    void CreatePara();
    void ResetPara();
    BgiProc::ESystemType GetSystemType();
    BgiProc::EProcType GetProcType();
    BgiProc::ESystemId GetSystemId();

    Parameter<RobotParaType::RobotPara>& RobotPara();
    Parameter<RobotParaType::RobotStatus>& RobotStatus();
    Parameter<RobotParaType::RobotPoint>& RobotPoint();

private:
    BgiProc::ESystemId m_SystemId;
};

#endif
